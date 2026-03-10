// Vince Petrelli All Rights Reserved


#include "Widgets/Item/SkillSlotWidgetBase.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"

void USkillSlotWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// 처음엔 쿨타임 바 숨김
	if (CooldownBar)
	{
		CooldownBar->SetPercent(0.0f);
		CooldownBar->SetVisibility(ESlateVisibility::Hidden);
	}
	if (Text_Cooldown) Text_Cooldown->SetVisibility(ESlateVisibility::Hidden);
}

void USkillSlotWidgetBase::InitSkillSlot(FName SkillTID)
{
	if (SkillTID.IsNone()) return;

	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	// 스킬 테이블에서 정보(아이콘) 가져오기
	FSkillTableRow* Row = TableSub->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, SkillTID);
	if (Row && Row->SkillIcon)
	{
		SkillIcon->SetBrushFromTexture(Row->SkillIcon);
		SkillIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

void USkillSlotWidgetBase::UpdateCooldown(float Current, float Max)
{
	if (!CooldownBar) return;

	// 쿨타임이 0보다 크면 (쿨타임 도는 중)
	if (Current > 0.0f && Max > 0.0f)
	{
		// 비율 계산 (예: 남은시간/전체시간)
		float Percent = Current / Max;

		CooldownBar->SetPercent(Percent);
		CooldownBar->SetVisibility(ESlateVisibility::Visible);

		// 남은 시간 텍스트 표시 (선택사항)
		if (Text_Cooldown)
		{
			// 소수점 1자리까지 표시
			Text_Cooldown->SetText(FText::AsNumber(float(int(Current * 10)) / 10.f));
			Text_Cooldown->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		// 쿨타임 없음 -> 숨김
		CooldownBar->SetVisibility(ESlateVisibility::Hidden);
		if (Text_Cooldown) Text_Cooldown->SetVisibility(ESlateVisibility::Hidden);
	}
}