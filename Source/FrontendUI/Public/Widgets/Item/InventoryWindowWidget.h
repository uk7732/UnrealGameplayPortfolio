// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryWindowWidget.generated.h"

class UInventoryWidgetBase;
class UEquipmentWidgetBase;

UCLASS()
class FRONTENDUI_API UInventoryWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 드롭 이벤트 재정의
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	// (기존 인벤토리 위젯 재사용)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidgetBase> WBP_InventoryGrid;

	// (새로 만든 장비 위젯)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEquipmentWidgetBase> WBP_Equipment;
	
};
