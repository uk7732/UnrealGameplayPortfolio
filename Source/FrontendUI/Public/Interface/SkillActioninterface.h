// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkillActioninterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USkillActioninterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

 // 스킬 액션 인터페이스 SkillAction 함수는 스킬을 실행하는 역할을 합니다.
class FRONTENDUI_API ISkillActioninterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool SkillAction(class ACC_CharacterBase* owner) { return false; }
};
