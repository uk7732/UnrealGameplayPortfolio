// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/GameDefines.h"
#include "Common/GameDatas.h"

#include "EquipmentWidgetBase.generated.h"

class UItemSlotWidgetBase;
class UEquipmentComponent;

UCLASS()
class FRONTENDUI_API UEquipmentWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 장비 컴포넌트와 연결 및 초기화
	void BindEquipment(UEquipmentComponent* EquipComp);

	UFUNCTION()
	void RefreshEquipmentUI();

protected:
	// 에디터에서 배치할 슬롯들 (변수명 일치 필수)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> Slot_Weapon_R;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> Slot_Weapon_L;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> Slot_Armor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemSlotWidgetBase> Slot_Helmet;	

private:
	TWeakObjectPtr<UEquipmentComponent> EquipmentComp;
};
