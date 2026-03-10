// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/GameDatas.h"
#include "AmmoWidgetBase.generated.h"

class UItemSlotWidgetBase;
class UTextBlock;

UCLASS()
class FRONTENDUI_API UAmmoWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// 재사용: 기존에 만드신 아이템 슬롯 위젯을 그대로 사용합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> AmmoSlot;

	// (선택사항) "Loaded" 같은 텍스트 표시용
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Label;

	// UI 갱신 함수
	UFUNCTION()
	void UpdateAmmoUI(int32 CurrentAmmo, int32 MaxAmmo);

	// 탄약 아이템의 테이블 ID (상수로 정의하거나 에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName AmmoItemID = FName("Ammo");
};
