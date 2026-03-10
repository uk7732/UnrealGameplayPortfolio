// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/GameDatas.h"
#include "Common/GameDefines.h" 


#include "ItemSlotWidgetBase.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class FRONTENDUI_API UItemSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 슬롯 데이터 초기화
	void InitSlot(const FItemData& InData); // 데이터 세팅


	// 슬롯 구별 
	enum class ESlotType
	{
		Inventory, // 인벤토리용 슬롯
		Equipment  // 장비창용 슬롯
	};

	// [여기 추가] 타입을 설정하는 함수들
	void SetSlotType(ESlotType InType) { SlotType = InType; }
	void SetEquipType(eItemEquipType InType) { EquipType = InType; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;

	FItemData ItemData;

	// 현재 슬롯의 인덱스 (InventoryWidget에서 할당)
	int32 MyIndex = -1;

	// 내 슬롯의 정체성을 저장할 변수
	ESlotType SlotType = ESlotType::Inventory; // 기본값
	eItemEquipType EquipType;                  // 장비 슬롯일 때 타입

	UFUNCTION()
	void OnSlotClicked();

	UFUNCTION()
	void OnSlotPressed();

	virtual void NativeConstruct() override;

public:
	// 인덱스 설정 함수
	void SetSlotIndex(int32 Index) { MyIndex = Index; }

	// 마우스 클릭 시
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// 드래그 감지 시
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// 무언가 드롭되었을 때
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};

