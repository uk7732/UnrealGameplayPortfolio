// Vince Petrelli All Rights Reserved


#include "Actor/RangedEffect/RangedEffectBase.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CC_CharacterBase.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ARangedEffectBase::ARangedEffectBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RangedEffectArea = CreateDefaultSubobject<USphereComponent>(TEXT("RangedEffectArea"));
	RangedEffectArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 쿼리만 허용 (물리적 충돌 방지)
	RangedEffectArea->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RangedEffectArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RangedEffectArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // 폰은 오버랩만 하도록 설정
	SetRootComponent(RangedEffectArea);

	RangedEffectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RangedEffectMesh"));
	RangedEffectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangedEffectMesh->SetupAttachment(RootComponent);

	PersistentEffectParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PersistentEffectParticles"));
	PersistentEffectParticles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PersistentEffectParticles->SetupAttachment(RootComponent);

	AreaNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AreaNiagaraComponent"));
	AreaNiagaraComponent->SetupAttachment(RootComponent);

	// 장판은 보통 생성되자마자 켜져야 하므로 AutoActivate가 기본값(true)인 게 편합니다.
	AreaNiagaraComponent->bAutoActivate = true;

}

void ARangedEffectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ARangedEffectBase::BeginPlay()
{
	Super::BeginPlay();

	if (!bIsDataInitialized)
	{
		bIsBeginPlay = true;
		return;
	}
	
	
	if (HasAuthority())
	{
		SkillTypeSetting();
	}


}
		


void ARangedEffectBase::SkillTypeSetting()
{
	if (DurationTime <= 0.0f)
	{
		// 즉발성 피해 로직 실행
		ApplyInstantDamage();

		// 즉시 액터 소멸 (이펙트가 재생될 시간을 줄 수도 있음)
		SetLifeSpan(RangedEffectBehaviorData.InstantDestroyDelay); // 0.1초 후 소멸 (이펙트가 잠시 보일 시간)
	}
	// ----------------------------------------------------
	// 2. **지속성 공격** (DurationTime이 0보다 클 때)
	// ----------------------------------------------------
	else
	{
		// 주기적인 피해 타이머 설정
		StartPeriodicDamageTimer();

		// 총 지속 시간 타이머 설정 (DurationTime 후 액터 소멸)
		SetLifeSpan(DurationTime);
	}
}

void ARangedEffectBase::InitializeFromData(const FName RangedEffectTID, AController* ownercontroller)
{
	if (!IsValid(ownercontroller))
		return;

	OwnerController = ownercontroller;

	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (nullptr == TableSub)
		return;

	FSkillTableRow* SkillTable = TableSub->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, RangedEffectTID);
	if (nullptr == SkillTable)
	{
		UE_LOG(LogTemp, Error, TEXT("RangedEffectBase '%s'를 DT_SkillTable에서 찾을 수 없습니다."), *RangedEffectTID.ToString());
		return;
	}

	RangedEffectBehaviorData = SkillTable->RangedEffectBehaviorData;
	//반경 설정함
	RangedEffectArea->SetSphereRadius(RangedEffectBehaviorData.EffectRadius);
	RangedEffectArea->UpdateOverlaps();
	DurationTime = SkillTable->RangedEffectBehaviorData.Duration;
	DurationTickDamage = RangedEffectBehaviorData.DurationTickDamage;

	ACC_CharacterBase* owner = Cast<ACC_CharacterBase>(OwnerController->GetPawn());
	float ownerAttack = owner->GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_ATTACK)->Value;
	float DefaultSkillDamage = SkillTable->Damage;
	
	if (SkillTable->Rates.Num() > 0)
	{
		for (auto& it : SkillTable->Rates)
		{
			if (it.Type == eAbilType::ABIL_TYPE_ATTACK)
			{
				float Rate = it.Rate;
				FinalAttackStatsSetting(DefaultSkillDamage, ownerAttack, Rate);
			}
			else if (it.Type == eAbilType::ABIL_TYPE_DEFENCE)
			{
				PercentArmorPen = it.Rate;
			}
		}
	}

	if(FinalDamage == 0)
		FinalAttackStatsSetting(DefaultSkillDamage, ownerAttack, 0);

	if (RangedEffectArea)
	{

		RangedEffectArea->SetSphereRadius(RangedEffectBehaviorData.EffectRadius);

		RangedEffectArea->SetRelativeScale3D(FVector(1.0f, 1.0f, 8.0f));

		

		RangedEffectArea->UpdateOverlaps();
	}


	if (AreaNiagaraComponent)
	{
		// 1. 에셋 할당 (테이블에 이펙트 정보가 있다면 여기서 직접 할당하는 게 안전합니다)
		// 만약 RangedEffectBehaviorData에 Niagara 변수가 있다면 그것을 사용하세요.
		if (AreaEffectAsset)
		{
			AreaNiagaraComponent->SetAsset(AreaEffectAsset);
		}

		// 2. 스케일 조절
		float TargetScale = RangedEffectBehaviorData.EffectRadius / 100.0f;
		AreaNiagaraComponent->SetRelativeScale3D(FVector(TargetScale, TargetScale, TargetScale / 8.0f));

		// 3. 강제 활성화
		AreaNiagaraComponent->Activate(true);
	}
	

	if (!bIsBeginPlay)
	{
		bIsDataInitialized = true;
		return;
	}

	SkillTypeSetting();
	
}
void ARangedEffectBase::FinalAttackStatsSetting(float DefaultSkillDamage, float OwnerStats, float Ratevalue)
{
	FinalDamage = DefaultSkillDamage + (OwnerStats * Ratevalue);
}


void ARangedEffectBase::ApplyInstantDamage()
{
	TArray<AActor*> OverlappingActors;

	RangedEffectArea->GetOverlappingActors(OverlappingActors);

	ACC_CharacterBase* Caster = Cast<ACC_CharacterBase>(GetInstigator());
	if (nullptr == Caster)
		return;

	for (AActor* TargetActor : OverlappingActors)
	{
		ACC_CharacterBase* TargetCharacter = Cast<ACC_CharacterBase>(TargetActor);

		if (nullptr == TargetCharacter)
			continue;

		if (TargetCharacter == Caster)
			continue;

		bool bIsSameTeam = (Caster->ActorHasTag(TEXT("Player")) && TargetActor->ActorHasTag(TEXT("Player"))) ||
							(Caster->ActorHasTag(TEXT("Monster")) && TargetActor->ActorHasTag(TEXT("Monster")));


		if (true == bIsSameTeam)
			continue;


		// FinalDamage(InstantDamage로 변경 권장) 값을 사용하여 피해 적용
		UGameplayStatics::ApplyDamage(
			TargetActor,
			FinalDamage, // 여기서 즉발 피해량(FinalDamage) 사용
			Caster->GetController(),
			this,
			UDamageType::StaticClass()
		);
	}

}

void ARangedEffectBase::StartPeriodicDamageTimer()
{
	if (DamageTickInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_DamageTick,           // 타이머 핸들
			this,                             // 이 클래스의 함수를 호출
			&ARangedEffectBase::ApplyPeriodicDamage, // 호출할 함수
			DamageTickInterval,               // 반복 주기 (블루프린트에서 설정한 값)
			true,                             // 반복 여부: true (반복 실행)
			0.0f                              // 초기 지연 시간 (즉시 틱 시작)
		);
	}
}

void ARangedEffectBase::ApplyPeriodicDamage()
{
	TArray<AActor*> OverlappingActors;
	RangedEffectArea->GetOverlappingActors(OverlappingActors);

	ACC_CharacterBase* Caster = Cast<ACC_CharacterBase>(GetInstigator());
	if (nullptr == Caster)
		return;

	for (AActor* TargetActor : OverlappingActors)
	{
		ACC_CharacterBase* TargetCharacter = Cast<ACC_CharacterBase>(TargetActor);

		if (nullptr == TargetCharacter)
			continue;

		if (TargetCharacter == Caster)
			continue;

		bool bIsSameTeam = (Caster->ActorHasTag(TEXT("Player")) && TargetActor->ActorHasTag(TEXT("Player"))) ||
			(Caster->ActorHasTag(TEXT("Monster")) && TargetActor->ActorHasTag(TEXT("Monster")));


		if (true == bIsSameTeam)
			continue;

		// DurationDamage(DurationTickDamage로 변경 권장) 값을 사용하여 피해 적용
		UGameplayStatics::ApplyDamage(
			TargetActor,
			DurationTickDamage, // 여기서 틱당 피해량(DurationDamage) 사용
			Caster->GetController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

void ARangedEffectBase::server_InitializeFromData_Implementation(const FName RangedEffectTID, AController* owner_controller)
{
	InitializeFromData(RangedEffectTID, owner_controller);
}


void ARangedEffectBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARangedEffectBase, FinalDamage);
	DOREPLIFETIME(ARangedEffectBase, DurationTickDamage);
	DOREPLIFETIME(ARangedEffectBase, PercentArmorPen);
	DOREPLIFETIME(ARangedEffectBase, DurationTime);
}