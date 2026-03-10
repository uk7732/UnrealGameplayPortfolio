// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/ActorBase.h"
#include "Common/GameDefines.h"
#include "Common/GameDatas.h"
#include "Interface/AttackStatsSource.h"
#include "ProjectileBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class FRONTENDUI_API AProjectileBase : public AActorBase, public IAttackStatsSource
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server ,Reliable)
	void server_InitializeFromData(const FName ProjectileTID, AController* ownercontroller);
	void server_InitializeFromData_Implementation(const FName ProjectileTID, AController* ownercontroller);

public:
	// 외부(캐릭터)에서 이 투사체를 데이터로 초기화하기 위한 함수
	void InitializeFromData(const FName ProjectileTID, AController* ownercontroller);

public:

	// 스킬 대미지는 캐릭터가 계산해서 넘겨주므로 public으로 유지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated, Category = "Projectile")
	float FinalDamage = 0.f;
	
	//방어력 관통 수치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	float PercentArmorPen = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	float Range = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	float Duration = 0.f;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	float CostMana = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	FProjectileBehaviorData ProjectileBehaviorData;

	// 발사자 정보 (피해 적용 및 팀 판별에 필수)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Projectile")
	AController* OwnerController;	

	UPROPERTY() //시작 위치
	FVector InitialLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:

	//자세한건 블루프린트 내에서 지정함
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ProjectileMesh;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* TrailParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* HitNiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* LaunchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShakeBase> HitCameraShakeClass;

	// 에디터에서 선택할 이펙트 에셋
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* TrailEffectAsset;

	// 미사일에 붙어있을 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	UNiagaraComponent* TrailComponent;


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:
	//폭발 반경 피해 로직
	UFUNCTION()
	void ApplyRadialDamageAndImpulse(float ExplosionRadius, float ExplosionDamageRatio, float ExplosionImpulse);

	UFUNCTION()
	void FinalAttackStatsSetting(float DefaultSkillDamage, float OwnerStats, float Ratevalue);


	virtual float GetArmorPenetrationPercent() const;

	// 1. 타이머 핸들: 이 핸들을 사용하여 타이머를 관리하고 취소합니다.
	FTimerHandle TimerHandle_LifeSpan;

	// 2. 타이머가 만료될 때 호출될 함수 (Projectile을 파괴)
	UFUNCTION()
	void OnProjectileExpired();

	UFUNCTION()
	// 3. (선택) 타이머를 설정하는 헬퍼 함수
	void StartLifeSpanTimer();

protected:
	UFUNCTION(NetMulticast, unreliable)
	void NetMultiCast_PlayNiagara(FVector Location, FRotator Rotation);
	void NetMultiCast_PlayNiagara_Implementation(FVector Location, FRotator Rotation);
};
