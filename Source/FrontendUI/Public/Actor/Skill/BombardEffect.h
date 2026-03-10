// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Actor/RangedEffect/RangedEffectBase.h"

#include "BombardEffect.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ABombardEffect : public ARangedEffectBase
{
	GENERATED_BODY()
	
protected:
	// 초기화: 테이블에서 "어떤 투사체(ProjectileClass)"를 쓸지 읽어옴
	virtual void InitializeFromData(const FName RangedEffectTID, AController* ownercontroller) override;

	// 핵심: 부모는 여기서 데미지를 주지만, 우리는 여기서 투사체를 스폰함
	virtual void ApplyPeriodicDamage() override;

private:
	// 스폰할 투사체 종류
	UPROPERTY()
	TSubclassOf<class AProjectileBase> RainProjectileClass;

	FName CachedSkillTID; // 스킬 ID 저장용

};
