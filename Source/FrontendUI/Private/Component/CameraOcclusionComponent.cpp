

#include "Component/CameraOcclusionComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Controllers/CC_PlayerController.h"

UCameraOcclusionComponent::UCameraOcclusionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(false);
}

void UCameraOcclusionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (false == GetWorld()->IsGameWorld())
		return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (nullptr == OwnerPawn)
		return;

	if (false == OwnerPawn->IsLocallyControlled())
		return;

	PlayerActor = OwnerPawn;
	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();

	if (nullptr == PlayerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraOcclusionComponent: 플레이어 연결 실패"));
		return;
	}
	
	if (nullptr == Camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraOcclusionComponent:  카메라 연결 실패"));
		return;
	}
}

void UCameraOcclusionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (nullptr == GetWorld()) 
		return;

	if (false == GetWorld()->IsGameWorld())
		return;

	UpdateOcclusion();
}

void UCameraOcclusionComponent::UpdateOcclusion()
{
	if (nullptr == PlayerActor || nullptr == Camera)
		return;

	PrevOccludedActors = CurrentOccludedActors;
	CurrentOccludedActors.Empty();

	FVector Start = Camera->GetComponentLocation();
	FVector End = PlayerActor->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerActor);
	Params.AddIgnoredActor(GetOwner());

	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, Params);

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		CurrentOccludedActors.Add(HitActor);

		TArray<UPrimitiveComponent*> Meshes;
		HitActor->GetComponents<UPrimitiveComponent>(Meshes);

		for (UPrimitiveComponent* Mesh : Meshes)
		{
			if (!Mesh || Mesh->GetNumMaterials() == 0) continue;

			UMaterialInstanceDynamic* MID = nullptr;

			if (MIDCache.Contains(Mesh))
				MID = MIDCache[Mesh];
			else
			{
				MID = Mesh->CreateDynamicMaterialInstance(0);
				MIDCache.Add(Mesh, MID);
			}

			if (MID)
				MID->SetScalarParameterValue(TEXT("Fade"), TargetFade);
		}
	}

	if (ACC_PlayerController* PC = Cast<ACC_PlayerController>(PlayerActor->GetController()))
	{
		PC->MouseOcclusionIgnoreActors = CurrentOccludedActors;
	}

	RestorePreviousWalls();
}

void UCameraOcclusionComponent::RestorePreviousWalls()
{
	for (AActor* Actor : PrevOccludedActors)
	{
		if (CurrentOccludedActors.Contains(Actor))
			continue;

		TArray<UPrimitiveComponent*> Meshes;
		Actor->GetComponents<UPrimitiveComponent>(Meshes);

		for (UPrimitiveComponent* Mesh : Meshes)
		{
			if (!MIDCache.Contains(Mesh))
				continue;

			MIDCache[Mesh]->SetScalarParameterValue(TEXT("Fade"), 1.0f);
		}
	}
}
