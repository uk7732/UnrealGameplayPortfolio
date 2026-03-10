// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryWidgetBase.generated.h"

class UFrontendCommonButtonBase;

UCLASS()
class FRONTENDUI_API UInventoryWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 인벤토리 변경 시 호출될 함수
    UFUNCTION()
    void RefreshUI();

protected:
    UPROPERTY(meta = (BindWidget))
    class UWrapBox* ItemGrid;

    // BP에서 설정할 슬롯 위젯 클래스 (WBP_ItemSlot)
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UItemSlotWidgetBase> SlotWidgetClass;

    // 정렬 버튼 (에디터에서 버튼 이름 Btn_Sort로 만들어야 함)
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Sort;
    /*TObjectPtr<UFrontendCommonButtonBase> Btn_Sort;*/

    UFUNCTION()
    void OnSortClicked();

};