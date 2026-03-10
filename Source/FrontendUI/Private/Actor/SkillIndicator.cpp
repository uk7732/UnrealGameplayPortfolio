// Vince Petrelli All Rights Reserved


#include "Actor/SkillIndicator.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASkillIndicator::ASkillIndicator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	// 1. 새로운 루트 컴포넌트 추가 (위치/이동 담당)
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 2. 데칼 컴포넌트를 루트에 부착 (회전/크기 담당)
	SkillDecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("IndicatorDecal"));
	SkillDecalComp->SetupAttachment(RootComponent);

	// 3. 자식인 데칼만 바닥을 보게 고정 (상대 회전)
	SkillDecalComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	// 기본 사이즈 설정
	SkillDecalComp->DecalSize = FVector(100.0f, 100.f, 100.0f);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CircleMaterial(TEXT("/Game/Decal/MT_DecalCircle.MT_DecalCircle"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BoxMaterial(TEXT("/Game/Decal/MT_DecalBox.MT_DecalBox"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CornMaterial(TEXT("/Game/Decal/MT_DecalCorn.MT_DecalCorn"));
	
	if (CircleMaterial.Succeeded()) { MaterialMap.Add(eDecalType::DECAL_CIRCLE, CircleMaterial.Object); }
	if (BoxMaterial.Succeeded())    { MaterialMap.Add(eDecalType::DECAL_BOX, BoxMaterial.Object); }
	if (CornMaterial.Succeeded())   { MaterialMap.Add(eDecalType::DECAL_CONE, CornMaterial.Object); }
}

void ASkillIndicator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkillIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkillIndicator::OnRep_DecalScale()
{
	if (SkillDecalComp)
	{
		SkillDecalComp->DecalSize = NetDecalScale;
	}
}

void ASkillIndicator::OnRep_ConeAngle()
{
	if (DecalMID)
	{
		DecalMID->SetScalarParameterValue(TEXT("ConeAngle"), NetConeAngle);
	}
}

void ASkillIndicator::SetupConeAngle(float radian)
{
	if (HasAuthority())
	{
		NetConeAngle = radian;
		
		if (DecalMID)
		{
			DecalMID->SetScalarParameterValue(TEXT("ConeAngle"), radian);
		}
	}
}

void ASkillIndicator::SetupIndicator(eDecalType Type,  FVector Scale, float Duration)
{
	if (HasAuthority())
	{
		NetDecalType = Type;

		UMaterialInterface* BaseMat = MaterialMap[NetDecalType];
		DecalMID = UMaterialInstanceDynamic::Create(BaseMat, this);
		SkillDecalComp->SetDecalMaterial(DecalMID);


		NetDecalScale = Scale;
		SkillDecalComp->DecalSize = Scale;

		

		SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));


		if (Duration > 0)
			SetLifeSpan(Duration);

		OnRep_DecalType();
		OnRep_DecalScale();
	}

}

void ASkillIndicator::OnRep_DecalType()
{
	if (!DecalMID && MaterialMap.Contains(NetDecalType))
	{
		DecalMID = UMaterialInstanceDynamic::Create(
			MaterialMap[NetDecalType], this);
		SkillDecalComp->SetDecalMaterial(DecalMID);
	}
}

void ASkillIndicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASkillIndicator, NetDecalType);
	DOREPLIFETIME(ASkillIndicator, NetConeAngle);
	DOREPLIFETIME(ASkillIndicator, NetDecalScale);
}


