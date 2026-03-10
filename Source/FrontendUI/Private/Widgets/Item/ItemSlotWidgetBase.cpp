// Vince Petrelli All Rights Reserved


#include "Widgets/Item/ItemSlotWidgetBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Controllers/CC_PlayerController.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "Widgets/Item/ItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h" 
#include "Component/EquipmentComponent.h"

#include "FrontendDebugHelper.h"



void UItemSlotWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();
    if (SlotButton) SlotButton->OnClicked.AddDynamic(this, &UItemSlotWidgetBase::OnSlotClicked);
    
}

FReply UItemSlotWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 좌클릭이고, 이 슬롯에 아이템이 있을 때만 드래그 가능
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && !ItemData.TID.IsNone())
    {
        // 언리얼 내장 함수: 드래그가 감지되면 NativeOnDragDetected를 호출해달라고 요청
        return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UItemSlotWidgetBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    // 택배 상자(Operation) 생성
    UItemDragDropOperation* DragOp = NewObject<UItemDragDropOperation>();

    // 데이터 채우기
    DragOp->ItemUID = ItemData.UID;

    // 내 슬롯 타입에 따라 SourceType 결정
    if (SlotType == ESlotType::Inventory)
    {
        DragOp->SourceType = EDragSourceType::Inventory;
        DragOp->SourceIndex = MyIndex;
    }
    else // Equipment
    {
        DragOp->SourceType = EDragSourceType::Equipment;
        DragOp->SourceEquipType = EquipType;
    }

    // TID 정보도 담기
    DragOp->ItemTID = ItemData.TID;
    
    // 드래그 중 마우스 따라다닐 위젯(Visual) 만들기
    // 현재 위젯과 똑같은 클래스를 하나 더 만들어서 보여줌
    UItemSlotWidgetBase* DragVisual = CreateWidget<UItemSlotWidgetBase>(this, GetClass());
    if (DragVisual)
    {        
        // 시각적으로 겹치지 않게 크기 강제 설정 등을 할 수 있음
        DragVisual->InitSlot(ItemData);

        // [핵심 수정] 드래그 중인 아이콘이 마우스 이벤트를 막지 않도록 설정!
        DragVisual->SetVisibility(ESlateVisibility::HitTestInvisible);
    } 

    // 만약 장비창에서 시작했다면, 어떤 부위인지 담아야 함
    if (SlotType == ESlotType::Equipment)
    {
        DragOp->SourceEquipType = EquipType; // .h에 선언해둔 EquipType 변수
    }
    else
    {
        DragOp->SourceIndex = MyIndex;
    }

    DragOp->DefaultDragVisual = DragVisual;
    DragOp->Pivot = EDragPivot::CenterCenter; // 마우스 중앙에 위치

    // 출력 파라미터에 할당
    OutOperation = DragOp;


}

bool UItemSlotWidgetBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // 들어온 택배가 내 시스템의 택배인지 확인
    UItemDragDropOperation* DragOp = Cast<UItemDragDropOperation>(InOperation);
    if (!DragOp) return false;

    // 인벤토리 컴포넌트 찾기
    ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwningPlayerState());
    if (!PS) return false;
    UInventoryComponent* InvComp = PS->GetInventory();
    UEquipmentComponent* EquipComp = PS->GetEquipment(); // 장비 컴포넌트도 필요

    // CASE 1: 인벤토리 -> 인벤토리 이동
    if (SlotType == ESlotType::Inventory)
    {
        // 1-A. 인벤토리 -> 인벤토리 (자리 바꾸기)
        if (DragOp->SourceType == EDragSourceType::Inventory)
        {
            if (DragOp->SourceIndex != MyIndex)
            {
                InvComp->RequestSwapItems(DragOp->SourceIndex, MyIndex);
                UE_LOG(LogTemp, Warning, TEXT("Drop 성공: %d -> %d"), DragOp->SourceIndex, MyIndex);
                return true;
            }
        }
        // 1-B. 장비창 -> 인벤토리 (장비 해제)
        else if (DragOp->SourceType == EDragSourceType::Equipment)
        {
            if (EquipComp)
            {
                EquipComp->RequestUnEquipItem(DragOp->SourceEquipType);
                return true;
            }
        }
    }
    // CASE 2: 장비창 -> 인벤토리 이동 (장비 해제)
    else if (SlotType == ESlotType::Equipment)
    {
        // 인벤토리 -> 장비창 (장착!)
        if (DragOp->SourceType == EDragSourceType::Inventory)
        {
            InvComp->Client_RequestUseItem(DragOp->ItemUID);
            return true;
        }
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UItemSlotWidgetBase::InitSlot(const FItemData& InData)
{
    ItemData = InData;
    
    // 아이템이 없으면 테이블 조회없이 리턴
    if (ItemData.TID.IsNone())
    {
        // 빈 슬롯 디자인 처리 (아이콘 숨기기 등)
        if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        if (ItemCountText) ItemCountText->SetVisibility(ESlateVisibility::Hidden);

        // 중요: 빈 슬롯이어도 드롭은 받아야 하므로 버튼은 Visible 상태여야 할 수도 있음.
        // 다만 드래그 시작은 막아야 함 (OnMouseButtonDown에서 이미 처리됨)
        return;
    }


    // 아이템이 있으니 보이게 설정
    if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::Visible);
    if (ItemCountText)
    {
        // 수량이 1보다 클 때만 표시
        if (ItemData.Count > 1)
        {
            ItemCountText->SetText(FText::AsNumber(ItemData.Count));
            ItemCountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            ItemCountText->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // 테이블에서 이미지 찾기
    UTableSubsystem* TableSub = UTableSubsystem::Get(this);
    if (TableSub)
    {
        FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemData.TID);
        // [수정 2] Row가 null이 아닐 때만 접근
        if (Row && Row->Image)
        {
            ItemIcon->SetBrushFromTexture(Row->Image);
        }
    }
}

void UItemSlotWidgetBase::OnSlotClicked()
{
    ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwningPlayerState());
    if (!PS) return;

    // 아이템이 없으면 클릭 무시 (빈 슬롯 클릭 방지)
    if (ItemData.TID.IsNone()) return;

    // 1. 인벤토리 슬롯 클릭 -> 아이템 사용/장착
    if (SlotType == ESlotType::Inventory)
    {
        if (UInventoryComponent* InvComp = PS->GetInventory())
        {
            InvComp->Client_RequestUseItem(ItemData.UID);
        }
    }
    // 2. 장비 슬롯 클릭 -> 장비 해제
    else if (SlotType == ESlotType::Equipment)
    {
        if (UEquipmentComponent* EquipComp = PS->GetEquipment())
        {
            // 내 슬롯 타입(무기/갑옷 등)을 보내서 해제 요청
            EquipComp->RequestUnEquipItem(EquipType);
        }
    }
}

void UItemSlotWidgetBase::OnSlotPressed()
{
    
}
