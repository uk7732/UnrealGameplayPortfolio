// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlotWidgetBase.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

UCLASS()
class FRONTENDUI_API USkillSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스킬 데이터 초기화 (아이콘 설정)
	void InitSkillSlot(FName SkillTID);

	// 쿨타임 업데이트 (0.0 ~ 1.0)
	// Current: 남은 시간, Max: 전체 쿨타임
	void UpdateCooldown(float Current, float Max);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	// 쿨타임이 돌 때 위를 덮을 프로그레스 바 (채우기 방식 사용)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CooldownBar;

	// (선택) 남은 시간 텍스트
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Cooldown;

	virtual void NativeConstruct() override;
};