// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Actor/ActorBase.h"
#include "Common/GameDatas.h"
#include "RangedEffectBase.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ARangedEffectBase : public AActorBase
{
	GENERATED_BODY()
	
public:
	ARangedEffectBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//즉발데미지
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Replicated, Category = "RangedEffect")
	float FinalDamage = 0.0f;

	//지속데미지
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Replicated, Category = "RangedEffect")
	float DurationTickDamage = 0.0f;

	//방어력 관통 수치
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Replicated, Category = "RangedEffect")
	float PercentArmorPen = 0.0f;

	//지속시간 (0일시 즉발공격) 0이 아닐시 지속적인 공격
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Replicated, Category = "RangedEffect")
	float DurationTime = 0.0f;

	//본인인지 아닌지 구분하기 위함임
	UPROPERTY()
	AController* OwnerController;

	UPROPERTY()
	FRangedEffectBehaviorData RangedEffectBehaviorData;

protected:
	bool bIsDataInitialized = false;
	bool bIsBeginPlay = false;
	void SkillTypeSetting();
	// **타이머 핸들:** 주기적인 피해 함수를 호출하는 데 사용
	FTimerHandle TimerHandle_DamageTick; 

	// **타이머 핸들:** 액터의 총 수명 관리에 사용 (옵션)
	FTimerHandle TimerHandle_LifeSpan; 

	//===========================블루프린트 설정 ===============================
	// 블루프린트에서 범위 반경을 지정
	//반복 주기(블루프린트 설정 필요)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageTickInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RangedEffectMesh;
	//콜리전 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* RangedEffectArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")	
	class UParticleSystemComponent* PersistentEffectParticles;
	
	// 장판이 생성될 때 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX & SFX")
	USoundBase* ActivationSound;

	// 장판이 사라질 때 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX & SFX")
	USoundBase* DeactivationSound;

	// 에디터에서 선택할 이펙트 에셋
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraSystem* AreaEffectAsset;

	// 미사일에 붙어있을 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	class UNiagaraComponent* AreaNiagaraComponent;
	//=========================================================================

public:
	UFUNCTION(Server, Reliable)
	virtual void server_InitializeFromData(const FName RangedEffectTID, AController* owner_controller);
	virtual void server_InitializeFromData_Implementation(const FName RangedEffectTID, AController* owner_controller);

	UFUNCTION()
	virtual void InitializeFromData(const FName RangedEffectTID, AController* owner_controller);


protected:
	void FinalAttackStatsSetting(float DefaultSkillDamage, float OwnerStats, float Ratevalue);

	UFUNCTION()
	void ApplyInstantDamage();
	
	UFUNCTION()
	void StartPeriodicDamageTimer();

	UFUNCTION()
	virtual void ApplyPeriodicDamage();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
