// Vince Petrelli All Rights Reserved


#include "Actor/Projectile/ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Instance/TableSubsystem.h"
#include "Common/GameDatas.h"
#include "Table/TableDatas.h" 
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values

AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);



	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh_New"));
	RootComponent = ProjectileMesh;
			
	ProjectileMesh->SetCollisionProfileName("Projectile");

	ProjectileMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	// 기본값 설정(데이터테이블에서 관리할 예정 최소 설정)	
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(RootComponent);


	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Trail"));
	TrailParticles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrailParticles->SetupAttachment(RootComponent);


}

void AProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (TrailComponent)
	{
		TrailComponent->Deactivate();

	}

	Super::EndPlay(EndPlayReason);

}


// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	//초기 위치 저장
	InitialLocation = GetActorLocation();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation(), GetActorRotation());
	}

	if (TrailComponent && TrailEffectAsset)
	{
		TrailComponent->SetAsset(TrailEffectAsset);
		TrailComponent->Activate(); // 이펙트 시작
	}

}

void AProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetInstigator() && ProjectileMesh)
	{
		ProjectileMesh->IgnoreActorWhenMoving(GetInstigator(), true);
	}

}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//Range(사거리)가 지정되어 있으면 HasAuthority() 를 통해 서버에서만 이 if문이 실행되게 함
	if (Range > 0.0f && HasAuthority())
	{
		float CurrentDistance = FVector::Dist(GetActorLocation(), InitialLocation);

		if (CurrentDistance >= Range)
		{
			Destroy();
		}
	}
}

void AProjectileBase::server_InitializeFromData_Implementation(const FName ProjectileTID, AController* ownercontroller)
{
	InitializeFromData(ProjectileTID, ownercontroller);
}

// 에셋과속성 로드&초기화
void AProjectileBase::InitializeFromData(const FName ProjectileTID, AController* ownercontroller)
{
	if (!IsValid(ownercontroller))
		return;

	OwnerController = ownercontroller;

	if (ownercontroller->GetPawn())
	{
		SetOwner(ownercontroller->GetPawn());
	}

	UTableSubsystem* tableSub = UTableSubsystem::Get(this);
	if (tableSub == nullptr) return;

	FSkillTableRow* projectileRow = tableSub->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, ProjectileTID);
	if (projectileRow == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileTID '%s'를 DT_SkillTable에서 찾을 수 없습니다."), *ProjectileTID.ToString());
		return;
	}

	ProjectileBehaviorData = projectileRow->ProjectileBehaviorData;
	CostMana = projectileRow->CostMana;

	AController* Controller = Cast<AController>(OwnerController);
	if (nullptr == Controller)
		return;

	ACC_CharacterBase* owner = Cast<ACC_CharacterBase>(Controller->GetPawn());
	float ownerAttack = owner->GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_ATTACK)->Value;

	// [수정된 데미지 공식 적용 파트]
	float DefaultSkillDamage = projectileRow->Damage;
	float CalculatedFinalDamage = DefaultSkillDamage; // 기본 데미지로 초기화
	PercentArmorPen = 0.0f;

	if (projectileRow->Rates.Num() > 0)
	{
		for (auto& it : projectileRow->Rates)
		{
			if (it.Type == eAbilType::ABIL_TYPE_ATTACK)
			{
				// 투사체 데미지 = 기본 데미지 + (내 공격력 * 테이블 공격 계수)
				CalculatedFinalDamage += (ownerAttack * it.Rate);
			}
			else if (it.Type == eAbilType::ABIL_TYPE_DEFENCE)
			{
				PercentArmorPen = it.Rate;
			}
		}
	}

	FinalDamage = CalculatedFinalDamage;

	Duration = projectileRow->ProjectileBehaviorData.Duration;
	StartLifeSpanTimer();  //Duration이 0 이면 지속시간 설정 없음으로 간주 됨

	Range = projectileRow->ProjectileBehaviorData.Range;
}


void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 서버에서만 실행 및 발사자/자신 충돌 무시
	if (!HasAuthority() || OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}
	

	ACC_CharacterBase* HitCharacter = Cast<ACC_CharacterBase>(OtherActor);
	bool bIsSameTeam = false;

	// 1. 발사자와 피격자가 같은 태그를 가지고 있다면 (팀킬 방지)
	// 예: 둘 다 "Monster" 태그를 가졌거나, 둘 다 "Player" 태그를 가진 경우
	if (HitCharacter)
	{
		AActor* Spawner = GetInstigator();
		if (Spawner)
		{
			bIsSameTeam = (Spawner->ActorHasTag(TEXT("Player")) && HitCharacter->ActorHasTag(TEXT("Player"))) ||
				          (Spawner->ActorHasTag(TEXT("Monster")) && HitCharacter->ActorHasTag(TEXT("Monster")));
		}
	}


	NetMultiCast_PlayNiagara(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	



	// 3. 행동 타입에 따른 분기
	switch (ProjectileBehaviorData.ActionType)
	{
	case eProjectileFinalAction::INSTANTLY_DESTROY:
		// 대상에게 피해를 입히고 즉시 사라짐 (총알/화살)
		if (!bIsSameTeam)
		{
			if (HitCharacter)
			{
				UGameplayStatics::ApplyDamage(
					OtherActor,
					FinalDamage,
					OwnerController,
					this,
					UDamageType::StaticClass()
				);
			
				ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetInstigator());
				if (player)
				{
					float ManaOnHitRestore = CostMana * 5;
					player->ApplyMP(ManaOnHitRestore);
				}
			}
		}

		SetActorHiddenInGame(true);        // 외형 숨기기 (클라이언트에 복제됨)
		SetActorEnableCollision(false);    // 충돌 끄기 (더 이상 OnHit 안 나게 함)

		if (TrailComponent)
		{
			TrailComponent->Deactivate();
		}

		// 0.1~0.2초 뒤에 서버에서 파괴 (RPC 패킷이 클라이언트에 도착할 시간을 벌어줌)
		SetLifeSpan(0.2f);
		break;

	case eProjectileFinalAction::EXPLODE_ONHIT:
		// 대상에게 피해를 입히고 광역 피해 후 사라짐
		ApplyRadialDamageAndImpulse(
			ProjectileBehaviorData.ExplosionRadius,
			ProjectileBehaviorData.ExplosionDamageRatio,
			ProjectileBehaviorData.ExplosionImpulse
		);
		Destroy();
		break;
	case eProjectileFinalAction::TIMED_EXPLOSION:
		// 충돌 여부와 상관없이 타이머로 폭발하므로, 충돌 시에는 아무것도 하지 않거나
		// 지형에 맞으면 파괴만 하도록 설정할 수 있습니다. (설계에 따라 다름)
		// 현재는 충돌 시 파괴하지 않고 계속 날아가 타이머로 만료되도록 'break' 처리.
		break;

	default:
		// 정의되지 않은 경우 파괴
		Destroy();
		break;
	}
	
}

void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileBase, FinalDamage);
	DOREPLIFETIME(AProjectileBase, Range);
	DOREPLIFETIME(AProjectileBase, Duration);
	DOREPLIFETIME(AProjectileBase, OwnerController);
	DOREPLIFETIME(AProjectileBase, ProjectileBehaviorData);
	DOREPLIFETIME(AProjectileBase, PercentArmorPen);
	DOREPLIFETIME(AProjectileBase, CostMana);
}

void AProjectileBase::ApplyRadialDamageAndImpulse(float ExplosionRadius, float ExplosionDamageRatio, float ExplosionImpulse)
{
	// 이 로직은 폭발 로직이므로 반드시 서버에서만 실행되어야 합니다.
	if (!HasAuthority())
	{
		return;
	}

	// 2. 피해 적용을 위한 초기 설정
	AController* DamageInstigator = OwnerController; // 발사자 컨트롤러
	TSubclassOf<UDamageType> DamageTypeClass = UDamageType::StaticClass(); // 사용할 데미지 타입 클래스 (필요 시 사용자 정의 클래스 사용)
	FVector Origin = GetActorLocation(); // 폭발 중심 위치

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,                                         // 월드 컨텍스트 (액터)
		FinalDamage,                                  // 최대 피해량 (폭발 중심의 피해)
		(float)FMath::FloorToInt(FinalDamage * 0.2f), // 최소 피해량 (피해 감소 후 최종적으로 남는 최소 피해)
		Origin,                                       // 폭발 중심 위치
		0.0f,                                         // InnerRadius (이 반경 내에서는 피해 감소 없음) - 보통 0.0f
		ExplosionRadius,                              // OuterRadius (이후 피해가 0이 되는 최대 반경)
		1.0f,                                         // DamageFalloff - 피해가 OuterRadius까지 얼마나 빠르게 감소할지 (1.0 = 선형 감소)
		DamageTypeClass,                              // 데미지 타입 클래스
		TArray<AActor*>(),                            // IgnoreActors - 무시할 액터 목록 (선택 사항)
		this,                                         // DamageCauser (피해를 유발한 액터)
		DamageInstigator,                             // InstigatorController (피해를 입힌 주체의 컨트롤러)
		ECC_GameTraceChannel1                         // TraceChannel - 폭발이 벽 뒤에 숨은 대상을 무시하도록 트레이스 체크
		);
	
	{
		TArray<FHitResult> HitResults;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);

		bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			Origin,
			Origin,
			FQuat::Identity,
			ECC_GameTraceChannel1,           // 캐릭터 중심
			Sphere
		);

		if (bHit)
		{
			for (auto& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor || HitActor == this)
					continue;

				UPrimitiveComponent* Comp = Hit.GetComponent();
				if (!Comp) continue;

				// 캐릭터면 넉백
				ACharacter* character = Cast<ACharacter>(HitActor);
				if (character)
				{
					UCharacterMovementComponent* MoveComp = character->FindComponentByClass<UCharacterMovementComponent>();
					if (MoveComp)
					{
						FVector Direction = (HitActor->GetActorLocation() - Origin).GetSafeNormal();
						MoveComp->AddImpulse(Direction * ExplosionImpulse, true);
					}
				}
				// 물리 객체면 RadialImpulse
				else if (Comp->IsSimulatingPhysics())
				{
					Comp->AddRadialImpulse(
						Origin,
						ExplosionRadius,
						ExplosionImpulse,
						ERadialImpulseFalloff::RIF_Linear,
						true
					);
				}
			}
		}
	}


}

void AProjectileBase::FinalAttackStatsSetting(float DefaultSkillDamage, float OwnerStats, float Ratevalue)
{
	FinalDamage = DefaultSkillDamage + (OwnerStats * Ratevalue);
}

float AProjectileBase::GetArmorPenetrationPercent() const
{
	return PercentArmorPen;
}



void AProjectileBase::OnProjectileExpired()
{
	if (HasAuthority())
	{
		switch (ProjectileBehaviorData.ActionType)
		{
		case eProjectileFinalAction::TIMED_EXPLOSION:
			// 시간 만료 시 폭발 로직 실행
			ApplyRadialDamageAndImpulse(
				ProjectileBehaviorData.ExplosionRadius,
				ProjectileBehaviorData.ExplosionDamageRatio,
				ProjectileBehaviorData.ExplosionImpulse
			);
			Destroy();
			break;
		default:
			// 다른 타입은 타이머가 만료되면 그냥 파괴
			Destroy();
			break;
		}
	}
}

void AProjectileBase::StartLifeSpanTimer()
{
	// Duration이 0보다 클 때만 타이머를 설정합니다.
	if (Duration > 0.0f)
	{
		// GetWorldTimerManager()는 현재 월드의 타이머 관리자를 가져옵니다.
		GetWorldTimerManager().SetTimer(
			TimerHandle_LifeSpan,         // 설정할 타이머 핸들
			this,                         // 타이머가 만료될 때 호출될 객체
			&AProjectileBase::OnProjectileExpired, // 호출될 함수
			Duration,                     // 몇 초 뒤에 호출할지 (Duration)
			false                         // 반복 여부 (단발성이므로 false)
		);
	}
}

void AProjectileBase::NetMultiCast_PlayNiagara_Implementation(FVector Location, FRotator Rotation)
{

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		HitNiagaraEffect,                  // Niagara System*
		Location,
		Rotation
	);
}
