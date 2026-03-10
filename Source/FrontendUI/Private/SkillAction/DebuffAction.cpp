// Vince Petrelli All Rights Reserved


#include "SkillAction/DebuffAction.h"
#include "Component/SkillComponent.h"

bool UDebuffAction::SkillAction(ACC_CharacterBase* owner)
{
	if (nullptr == owner)
		return false;

	FSkillTableRow SkillTable = owner->GetCurrentSkillData();
	FName SkillTID = owner->GetCurrentSkillTID();

	if (SkillTable.MontageID.IsNone())
		return false;

	FDebuffActionData DebuffData = SkillTable.DebuffActionData;

	USkillComponent* skillComp = owner->GetSkillComp();
	if (nullptr == skillComp)
		return false;
	//쿨타임 리셋
	skillComp->CurrentSkillResetCoolDown();
	//마나소모
	owner->ApplyMP(-SkillTable.CostMana);

	if (DebuffData.MinusAttackStat != 0.0f)
	{
		FAbilityData data;
		data.TID = SkillTID;
		data.Type = eAbilType::ABIL_TYPE_ATTACK;
		data.ModifierType = eAbilModifierType::ABIL_MODIFIER_MINUS;
		data.DurationTime = DebuffData.Duration;
		data.Value = DebuffData.MinusAttackStat;
		owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_DEBUFF, data);
	}

	if (DebuffData.MinusDefenceStat != 0.0f)
	{
		FAbilityData data;
		data.TID = SkillTID;
		data.Type = eAbilType::ABIL_TYPE_DEFENCE;
		data.ModifierType = eAbilModifierType::ABIL_MODIFIER_MINUS;
		data.DurationTime = DebuffData.Duration;
		data.Value = DebuffData.MinusDefenceStat;
		owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, data);
	}

	for (auto& it : DebuffData.Rates)
	{
		if (it.Rate == 0.0f)
			continue;

		if (it.Type == eAbilType::ABIL_TYPE_ATTACK)
		{
			FAbilityData data;
			data.TID = SkillTID;
			data.Type = eAbilType::ABIL_TYPE_ATTACK;
			data.ModifierType = eAbilModifierType::ABIL_MODIFIER_PERCENT;
			data.DurationTime = DebuffData.Duration;
			data.Value = it.Rate;
			owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_DEBUFF, data);
		}
		else
		{
			FAbilityData data;
			data.TID = SkillTID;
			data.Type = eAbilType::ABIL_TYPE_DEFENCE;
			data.ModifierType = eAbilModifierType::ABIL_MODIFIER_PERCENT;
			data.DurationTime = DebuffData.Duration;
			data.Value = it.Rate;
			owner->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_DEBUFF, data);
		}
	}


	return false;
}
