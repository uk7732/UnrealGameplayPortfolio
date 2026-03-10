// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SkillAction/SkillActionBase.h"
#include "ProjectileAction.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UProjectileAction : public USkillActionBase
{
	GENERATED_BODY()
	
	//ISkillActioninterface 함수 스킬 시전시 사용
	virtual bool SkillAction(class ACC_CharacterBase* owner) override;

};
