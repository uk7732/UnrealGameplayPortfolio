// Vince Petrelli All Rights Reserved


#include "Widgets/Item/InventoryWidgetBase.h"
#include "Widgets/Item/ItemSlotWidgetBase.h"
#include "Components/WrapBox.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "Components/Button.h" 

void UInventoryWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    // PlayerState의 InventoryComponent 찾아서 델리게이트 바인딩
    if (APlayerState* PS = GetOwningPlayerState())
    {
        if (ACC_PlayerState* FrontendPS = Cast<ACC_PlayerState>(PS))
        {
            if (UInventoryComponent* InvComp = FrontendPS->GetInventory())
            {
                // 인벤토리가 변할 때마다(OnRep) RefreshUI 호출, 중복 방지를 위한 Remove 먼저 호출
                //InvComp->OnInventoryUpdated.RemoveDynamic(this, &UInventoryWidgetBase::RefreshUI);
                //InvComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidgetBase::RefreshUI);
                InvComp->OnInventoryUpdated.AddUniqueDynamic(this, &UInventoryWidgetBase::RefreshUI);

                RefreshUI(); // 초기화 시 한 번 실행
            }
        }
    }
    // 정렬 버튼 이벤트 연결
    if (Btn_Sort)
    {
        Btn_Sort->OnClicked.AddDynamic(this, &UInventoryWidgetBase::OnSortClicked);
    }
}

void UInventoryWidgetBase::RefreshUI()
{
    if (!ItemGrid || !SlotWidgetClass) return;

    ItemGrid->ClearChildren();

    ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwningPlayerState());
    if (!PS || !PS->GetInventory()) return;

    UInventoryComponent* InvComp = PS->GetInventory();

    // 1. 데이터 가져오기
    const TArray<FItemData>& Items = InvComp->GetInventoryItems();
    int32 MaxSlots = InvComp->GetInventoryMaxSpace(); // 24

    // 2. [중요] 반복문은 '최대 칸 수'만큼 딱 한 번만 돌려야 합니다!
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        UItemSlotWidgetBase* ItemSlot = CreateWidget<UItemSlotWidgetBase>(this, SlotWidgetClass);

        if (ItemSlot)
        {
            // [핵심 1] 슬롯에게 자신의 번호표(인덱스)를 줍니다. (이게 있어야 드래그 앤 드롭 계산 가능)
            ItemSlot->SetSlotIndex(i);

            // [핵심 2] 현재 칸(i)에 아이템 데이터가 있는지 확인
            if (Items.IsValidIndex(i) && !Items[i].TID.IsNone())
            {
                // 아이템이 있으면 그 정보로 초기화
                ItemSlot->InitSlot(Items[i]);
            }
            else
            {
                // 없으면 빈 껍데기로 초기화
                FItemData EmptyItem;
                EmptyItem.TID = NAME_None;
                ItemSlot->InitSlot(EmptyItem);
            }

            ItemGrid->AddChildToWrapBox(ItemSlot);
        }
    }
}

void UInventoryWidgetBase::OnSortClicked()
{
    if (ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwningPlayerState()))
    {
        if (UInventoryComponent* Inv = PS->GetInventory())
        {
            Inv->RequestSortInventory();
        }
    }
}
