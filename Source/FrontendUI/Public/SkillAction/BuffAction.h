// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SkillAction/SkillActionBase.h"
#include "BuffAction.generated.h"

/**
 * 
 */
UCLASS()
class UBuffAction : public USkillActionBase
{
	GENERATED_BODY()
	

	virtual bool SkillAction(class ACC_CharacterBase* owner) override;
};
