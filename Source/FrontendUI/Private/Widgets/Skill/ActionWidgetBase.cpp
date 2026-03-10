// Vince Petrelli All Rights Reserved


#include "Widgets/Skill/ActionWidgetBase.h"
#include "Widgets/Item/SkillSlotWidgetBase.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Component/SkillComponent.h"

void UActionWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	// 초기화 시 한번 업데이트
	UpdateSlotState();

	// 1. 초기 생성시 캐릭터 확인
	APawn* OwningPawn = GetOwningPlayerPawn();
	ACC_PlayerCharacter* PlayerChar = Cast<ACC_PlayerCharacter>(OwningPawn);

	if (PlayerChar)
	{
		// 캐릭터가 있는데 UI가 숨겨져 있다면 -> 보이게 설정
		if (GetVisibility() == ESlateVisibility::Collapsed || GetVisibility() == ESlateVisibility::Hidden)
		{
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		// 쿨타임 및 아이콘 갱신 로직 실행
		UpdateSlotState();
	}
	else
	{
		// 캐릭터가 사라졌는데 UI가 보인다면 -> 숨김
		if (GetVisibility() != ESlateVisibility::Collapsed)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UActionWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// 매 프레임 쿨타임 갱신
	UpdateSlotState();
}

void UActionWidgetBase::UpdateSlotState()
{
	if (!SkillSlot) return;

	APawn* OwningPawn = GetOwningPlayerPawn();
	ACC_PlayerCharacter* Player = Cast<ACC_PlayerCharacter>(OwningPawn);
	if (!Player) return;

	USkillComponent* SkillComp = Player->GetSkillComp();
	if (!SkillComp) return;

	FName TargetTID = NAME_None;

	// 1. ActionType에 따라 어떤 스킬 TID인지 확인
	switch (ActionType)
	{
	case EActionSlotType::NormalAction:
		TargetTID = SkillComp->GetNormalSkillTID();
		break;
	case EActionSlotType::SubNormalAction:
		TargetTID = SkillComp->GetSubNormalSkillTID();
		break;
	case EActionSlotType::DashAction:
		// [주의] 대시를 스킬 컴포넌트에 "Dash"라는 이름의 스킬로 등록해야 쿨타임 추적이 가능합니다.
		TargetTID = FName("Dash");
		break;
	case EActionSlotType::SkillSlot_1:
		TargetTID = SkillComp->GetSkillTIDInSlot(1);
		break;
	case EActionSlotType::SkillSlot_2:
		TargetTID = SkillComp->GetSkillTIDInSlot(2);
		break;
	}

	// 2. 스킬이 바뀌었다면 아이콘 갱신 (초기화)
	if (TargetTID != CachedSkillTID)
	{
		CachedSkillTID = TargetTID;
		SkillSlot->InitSkillSlot(CachedSkillTID);
	}

	// 3. 쿨타임 정보 가져와서 UI 갱신 (Dimming 효과)
	if (!TargetTID.IsNone())
	{
		FSkillData* SkillData = SkillComp->FindSkill(TargetTID);
		if (SkillData)
		{
			// 현재 쿨타임과 전체 쿨타임을 전달
			SkillSlot->UpdateCooldown(SkillData->CurrentCoolDown, SkillData->CoolDown);
		}
	}
}