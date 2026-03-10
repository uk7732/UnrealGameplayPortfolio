// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SkillAction/SkillActionBase.h"
#include "RangedAction.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API URangedAction : public USkillActionBase
{
	GENERATED_BODY()
	

	virtual bool SkillAction(class ACC_CharacterBase* owner) override;

};
