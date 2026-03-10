// Vince Petrelli All Rights Reserved


#include "Actor/Item/ItemBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Table/TableDatas.h"
#include "Instance/Admin4_GameInstance.h"
#include "Instance/TableSubsystem.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	SetRootComponent(MeshComponent);
	
	// 아이템 Custom Depth 설정
	MeshComponent->SetRenderCustomDepth(true);
	MeshComponent->SetCustomDepthStencilValue(3); // 예: 3 = 아이템 (노란색 등)

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetGenerateOverlapEvents(true);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);

	ShineEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	ShineEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShineEffect->SetupAttachment(RootComponent);
}

void AItemBase::InitializedItemData(const FName& ItemTID, int64 InUID, int32 InCount)
{
	server_InitializedItemData(ItemTID, InUID, InCount);
}

void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		// 1. 서버 권한 및 중복 획득 체크(필수)
		// 서버가 아니거나, 이미 획득 처리 중이라면 함수 종료
		if (!HasAuthority() || bIsPendingDestroy)
		{
			return;
		}
		
		ACC_PlayerCharacter* Character = Cast<ACC_PlayerCharacter>(OtherActor);
		if (nullptr == Character)
			return;

		APlayerState* PS = Character->GetPlayerState();

		 UInventoryComponent* Inventory = PS ? PS->FindComponentByClass<UInventoryComponent>() : nullptr;
		 if (Inventory)
		 {
			 Inventory->TryAddItem(ItemData, this); 
			 bIsPendingDestroy = true;
		 }
}

void AItemBase::OnRep_ItemData()
{
	UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
	if (tablesystem)
	{
		FItemTableRow* ItemTable = tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemData.TID);
		if (ItemTable)
		{
			MeshComponent->SetStaticMesh(ItemTable->Mesh);
		}
	}
}

void AItemBase::server_InitializedItemData_Implementation(const FName& ItemTID, int64 InUID, int32 InCount)
{
	UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
	if (nullptr == tablesystem)
		return;

	FItemTableRow* ItemTable = tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemTID);
	if (nullptr == ItemTable)
		return;

	MeshComponent->SetStaticMesh(ItemTable->Mesh);
	ItemData.UID = InUID;
	ItemData.TID = ItemTID;
	ItemData.Count = InCount;

}


void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AItemBase, ItemData);
}

