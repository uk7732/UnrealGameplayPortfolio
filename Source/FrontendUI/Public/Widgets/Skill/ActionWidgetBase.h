// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionWidgetBase.generated.h"

class USkillSlotWidgetBase;

// 어떤 액션을 추적할지 결정하는 열거형
UENUM(BlueprintType)
enum class EActionSlotType : uint8
{
	NormalAction,    // 좌클릭
	SubNormalAction, // 우클릭
	DashAction,      // 대시 (Space or Shift)
	SkillSlot_1,     // 숫자키 1
	SkillSlot_2      // 숫자키 2
};

UCLASS()
class FRONTENDUI_API UActionWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// 시각적 표현을 담당할 자식 위젯
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillSlotWidgetBase> SkillSlot;

	// 에디터에서 설정: 이 위젯이 어떤 스킬을 보여줄지 선택
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	EActionSlotType ActionType = EActionSlotType::NormalAction;

private:
	// 현재 추적 중인 스킬 TID (캐싱용)
	FName CachedSkillTID;

	void UpdateSlotState();
};
