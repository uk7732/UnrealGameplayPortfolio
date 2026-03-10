// Vince Petrelli All Rights Reserved


#include "Widgets/Item/EquipmentWidgetBase.h"
#include "Widgets/Item/ItemSlotWidgetBase.h"
#include "Component/EquipmentComponent.h"

void UEquipmentWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquipmentWidgetBase::BindEquipment(UEquipmentComponent* EquipComp)
{
    // [수정 1] 매개변수로 들어온 EquipComp가 유효한지 체크해야 함
    if (!EquipComp) return;

    // [수정 2] 멤버 변수에 저장
    EquipmentComp = EquipComp;

    // [수정 3] 델리게이트 연결 (이게 있어야 나중에 장비 바뀌면 UI가 자동으로 바뀜)
    //EquipmentComp->OnEquipmentUpdated.RemoveDynamic(this, &UEquipmentWidgetBase::RefreshEquipmentUI);
    //EquipmentComp->OnEquipmentUpdated.AddDynamic(this, &UEquipmentWidgetBase::RefreshEquipmentUI);
    EquipmentComp->OnEquipmentUpdated.AddUniqueDynamic(this, &UEquipmentWidgetBase::RefreshEquipmentUI);


    // [수정 4] 슬롯들의 '정체성' 부여 (한 번만 하면 됨)
    if (Slot_Weapon_R)
    {
        Slot_Weapon_R->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
        Slot_Weapon_R->SetEquipType(eItemEquipType::WEAPON_R);
    }
    if (Slot_Weapon_L)
    {
        Slot_Weapon_L->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
        Slot_Weapon_L->SetEquipType(eItemEquipType::WEAPON_L);
    }
    if (Slot_Armor)
    {
        Slot_Armor->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
        Slot_Armor->SetEquipType(eItemEquipType::ARMOR);
    }
    if (Slot_Helmet)
    {
        Slot_Helmet->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
        Slot_Helmet->SetEquipType(eItemEquipType::HELMET);
    }

    // 화면 갱신 요청
    RefreshEquipmentUI();
}

void UEquipmentWidgetBase::RefreshEquipmentUI()
{
    // 컴포넌트가 연결되어 있는지 확인
    if (!EquipmentComp.IsValid()) return;

    // 1. 먼저 모든 슬롯을 빈 상태로 초기화 (잔상 제거)
    FItemData EmptyItem; // TID=None
    if (Slot_Weapon_R) Slot_Weapon_R->InitSlot(EmptyItem);
    if (Slot_Weapon_L)Slot_Weapon_L->InitSlot(EmptyItem);
    if (Slot_Armor)  Slot_Armor->InitSlot(EmptyItem);
    if (Slot_Helmet) Slot_Helmet->InitSlot(EmptyItem);

    // 2. 현재 장착된 아이템 리스트 가져오기
    const TArray<FItemEquipSlotData>& CurrentEquips = EquipmentComp->GetEquipSlots();

    // 3. 루프 돌면서 채우기
    for (const FItemEquipSlotData& SlotData : CurrentEquips)
    {
        // UI 표시를 위해 FItemData로 변환
        FItemData DisplayData;
        DisplayData.TID = SlotData.ItemEquipData.TID;
        DisplayData.UID = SlotData.ItemEquipData.UID;
        DisplayData.Count = 1;

        // 슬롯 타입에 따라 맞는 위젯에 Init
        switch (SlotData.Type)
        {
        case eItemEquipType::WEAPON_R: // 무기 (오른손)
            if (Slot_Weapon_R) Slot_Weapon_R->InitSlot(DisplayData);
            break;
        case eItemEquipType::WEAPON_L: // 무기 (왼손)
            if (Slot_Weapon_L) Slot_Weapon_L->InitSlot(DisplayData);
            break;
        case eItemEquipType::ARMOR: // 갑옷
            if (Slot_Armor) Slot_Armor->InitSlot(DisplayData);
            break;

        case eItemEquipType::HELMET: // 투구
            if (Slot_Helmet) Slot_Helmet->InitSlot(DisplayData);
            break;
        }
    }
}
