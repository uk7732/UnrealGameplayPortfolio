// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/GameDatas.h"
#include "PotionWidgetBase.generated.h"

class UItemSlotWidgetBase;
class UTextBlock;

UCLASS()
class FRONTENDUI_API UPotionWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// 재사용: 아이템 슬롯 위젯 (아이콘 + 갯수 표시용)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> PotionSlot;

	// (선택사항) 단축키 텍스트 (예: "Q" 또는 "1")
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_KeyLabel;

	// UI 갱신 함수 (인벤토리가 변경될 때 호출됨)
	UFUNCTION()
	void UpdatePotionUI();

	// 추적할 포션의 아이템 ID (에디터에서 "Potion_HP_01" 등으로 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName PotionItemID = FName("Potion_HP_01");
};
