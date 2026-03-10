// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interface/SkillActionInterface.h"
#include "Interface/AttackStatsSource.h"
#include "Table/TableDatas.h"
#include "SkillActionBase.generated.h"

struct FSkillTableRow;

/**
 * 
 */
UCLASS()
class FRONTENDUI_API USkillActionBase : public UObject, public ISkillActioninterface 
{
	GENERATED_BODY()
	
protected:


public:
	//ISkillActioninterface 함수 스킬 시전시 사용
	virtual bool SkillAction(class ACC_CharacterBase* owner) override;
};

