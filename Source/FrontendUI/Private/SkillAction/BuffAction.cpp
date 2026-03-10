// Vince Petrelli All Rights Reserved


#include "SkillAction/BuffAction.h"
#include "Component/SkillComponent.h"


bool UBuffAction::SkillAction(ACC_CharacterBase* owner)
{
	if (nullptr == owner)
		return false;

	FSkillTableRow SkillTable = owner->GetCurrentSkillData();
	FName SkillTID = owner->GetCurrentSkillTID();
	if (SkillTable.MontageID.IsNone())
		return false;

	FBuffActionData BuffData = SkillTable.BuffActionData;

	USkillComponent* skillComp = owner->GetSkillComp();
	if (nullptr == skillComp)
		return false;
	//쿨타임 리셋
	skillComp->CurrentSkillResetCoolDown();
	//마나소모
	owner->ApplyMP(-SkillTable.CostMana);


	if (BuffData.AddAttackStat != 0.0f)
	{
		FAbilityData data;
		data.TID = SkillTID;
		data.Type = eAbilType::ABIL_TYPE_ATTACK;
		data.ModifierType = eAbilModifierType::ABIL_MODIFIER_ADD;
		data.DurationTime = BuffData.Duration;
		data.Value = BuffData.AddAttackStat;
		owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, data);
	}

	if (BuffData.AddDefenceStat != 0.0f)
	{
		FAbilityData data;
		data.TID = SkillTID;
		data.Type = eAbilType::ABIL_TYPE_DEFENCE;
		data.ModifierType = eAbilModifierType::ABIL_MODIFIER_ADD;
		data.DurationTime = BuffData.Duration;
		data.Value = BuffData.AddDefenceStat;
		owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, data);
	}

	for (auto& it : BuffData.Rates)
	{
		if (it.Rate == 0.0f)
			continue;

		if (it.Type == eAbilType::ABIL_TYPE_ATTACK)
		{
			FAbilityData data;
			data.TID = SkillTID;
			data.Type = eAbilType::ABIL_TYPE_ATTACK;
			data.ModifierType = eAbilModifierType::ABIL_MODIFIER_PERCENT;
			data.DurationTime = BuffData.Duration;
			data.Value = it.Rate;
			owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, data);
		}
		else
		{
			FAbilityData data;
			data.TID = SkillTID;
			data.Type = eAbilType::ABIL_TYPE_DEFENCE;
			data.ModifierType = eAbilModifierType::ABIL_MODIFIER_PERCENT;
			data.DurationTime = BuffData.Duration;
			data.Value = it.Rate;
			owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, data);
		}
	}



	return true;
}
