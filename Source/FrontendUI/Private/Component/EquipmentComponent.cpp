// Vince Petrelli All Rights Reserved


#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Instance/Admin4_GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CC_CharacterBase.h" 
#include "GameMode/CC_PlayerState.h"
#include "Component/AbilityComponent.h"
#include "Instance/TableSubsystem.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEquipmentComponent::SetEquipSlotItem(eItemEquipType Type, const FItemData& ItemData)
{
	server_SetEquipSlotItem(Type, ItemData);
	
	if (OnEquipmentUpdated.IsBound())
		OnEquipmentUpdated.Broadcast();
}

void UEquipmentComponent::OnRep_EquipSlots()
{
	// 1. 내 주인(PlayerState)을 찾는다.
	ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwner());
	if (!PS) return;

	// 2. PlayerState가 빙의된 캐릭터(Pawn)를 찾는다.
	ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(PS->GetPawn());
	if (Character)
	{
#pragma region Weapon_R
		// 3. 무기 슬롯(SWORD)에 장착된 게 있는지 찾는다.
		// (람다식을 써서 Type이 SWORD인 데이터를 찾음)
		FItemEquipSlotData* RightWeapon = EquipSlots.FindByPredicate([](const FItemEquipSlotData& Slot) {
			return Slot.Type == eItemEquipType::WEAPON_R;
			});
		Character->UpdateWeaponVisual(RightWeapon ? RightWeapon->ItemEquipData.TID : NAME_None, eItemEquipType::WEAPON_R);
#pragma endregion
#pragma region Weapon_L
		FItemEquipSlotData* LeftWeapon = EquipSlots.FindByPredicate([](const FItemEquipSlotData& Slot) {
			return Slot.Type == eItemEquipType::WEAPON_L;
			});
		Character->UpdateWeaponVisual(LeftWeapon ? LeftWeapon->ItemEquipData.TID : NAME_None, eItemEquipType::WEAPON_L);

#pragma endregion
#pragma region Helmet
		FItemEquipSlotData* Helmet = EquipSlots.FindByPredicate([](const FItemEquipSlotData& Slot) {
			return Slot.Type == eItemEquipType::HELMET;
			});
		Character->UpdateWeaponVisual(Helmet ? Helmet->ItemEquipData.TID : NAME_None, eItemEquipType::HELMET);

#pragma endregion
#pragma region Armor
		FItemEquipSlotData* Armor = EquipSlots.FindByPredicate([](const FItemEquipSlotData& Slot) {
			return Slot.Type == eItemEquipType::ARMOR;
			});
		Character->UpdateWeaponVisual(Armor ? Armor->ItemEquipData.TID : NAME_None, eItemEquipType::ARMOR);
	}
#pragma endregion
	// 5. UI에게도 변경 사실 알림
	if (OnEquipmentUpdated.IsBound())
	{
		OnEquipmentUpdated.Broadcast();
	}
}

void UEquipmentComponent::RequestUnEquipItem(eItemEquipType SlotType)
{
	server_UnEquipItem(SlotType);
}


void UEquipmentComponent::server_SetEquipSlotItem_Implementation(eItemEquipType Type, const FItemData& ItemData)
{
	////todo : 해당 EquipSlot 에 아이템이 장착되어 있을 경우 해당 InventorySlot에 장착되어 있는 아이템을 다시 반환해줘야함
	////
	//FItemEquipSlotData* SlotData = EquipSlots.FindByKey(Type);

	//UInventoryComponent* Inventory = Cast<APlayerState_Frontend>(GetOwner())->GetInventory();
	//if (nullptr == Inventory)
	//	return;
	//
	//FName EquipItemTID = ItemData.TID;
	//int64 EquipItemUID = ItemData.UID;
	//int64 EquipItemEnchant = ItemData.Enchant;

	//Inventory->TryEraseIten(EquipItemTID, EquipItemUID);
	//if(nullptr != SlotData)
	//{
	//	//todo : 해당 슬롯에 아이템 장착되어 있음
	//	FItemData* itemData = {};
	//	itemData->UID = SlotData->ItemEquipData.UID;
	//	itemData->TID = SlotData->ItemEquipData.TID;
	//	itemData->Enchant = SlotData->ItemEquipData.Enchant;
	//	
	//	Inventory->TryAddItem(*itemData);
	//}

	//FItemEquipSlotData* NewSlotData = {};
	//NewSlotData->Type = Type;
	//NewSlotData->ItemEquipData.TID = ItemData.TID;
	//NewSlotData->ItemEquipData.UID = ItemData.UID;
	//NewSlotData->ItemEquipData.Enchant = ItemData.Enchant;

	//EquipSlots.Add(*NewSlotData);
	//
	//// 서버에서 외형 호출
	//OnRep_EquipSlots();
	 // 1. 해당 슬롯에 이미 아이템이 있는지 확인
	FItemEquipSlotData* SlotData = EquipSlots.FindByKey(Type);

	UInventoryComponent* Inventory = Cast<ACC_PlayerState>(GetOwner())->GetInventory();
	if (nullptr == Inventory) return;

	ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwner());
	if (nullptr == PS)
		return;

	ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(PS->GetPawn());
	if (nullptr == Character)
		return;

	UAbilityComponent* Abil = Character->GetAbilityComp();
	if (nullptr == Abil)
		return;

	// 장착하려는 아이템 정보를 미리 저장
	FName EquipItemTID = ItemData.TID;
	int64 EquipItemUID = ItemData.UID;

	UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
	if (nullptr == Tablesystem)
		return;

	FItemTableRow* ItemRow = Tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, EquipItemTID);
	if (nullptr == ItemRow)
		return;

	FAbilGroupTableRow* AbilGroup = Tablesystem->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, ItemRow->AbilGroupTID);
	if (nullptr == AbilGroup)
	{
		UE_LOG(LogTemp, Error, TEXT("해당 아이템 Ability 정보가 비어 있습니다"));
	}

	// [수정] 2. 기존 아이템이 있다면 인벤토리로 반환
	if (nullptr != SlotData)
	{
		FItemData OldItemData; // 포인터가 아닌 일반 구조체로 선언해야 안전합니다.
		OldItemData.UID = SlotData->ItemEquipData.UID;
		OldItemData.TID = SlotData->ItemEquipData.TID;
		OldItemData.Enchant = SlotData->ItemEquipData.Enchant;
		OldItemData.Count = 1;
		

		Inventory->TryAddItem(OldItemData); // 인벤토리에 다시 넣어줌

		Abil->EraseAbilData(eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP, OldItemData.TID);

		// 기존 데이터 삭제 (교체 준비)
		EquipSlots.RemoveAll([Type](const FItemEquipSlotData& Data) { return Data.Type == Type; });
	}

	// 3. 인벤토리에서 새로 장착할 아이템 제거
	Inventory->TryEraseIten(EquipItemTID, EquipItemUID);

	// 4. 장비 슬롯에 새 아이템 추가
	FItemEquipSlotData NewSlotData;
	NewSlotData.Type = Type;
	NewSlotData.ItemEquipData.TID = EquipItemTID;
	NewSlotData.ItemEquipData.UID = EquipItemUID;
	NewSlotData.ItemEquipData.Enchant = ItemData.Enchant;

	if (nullptr != AbilGroup)
	{
		Abil->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP, AbilGroup->Abils);
	}
	
	EquipSlots.Add(NewSlotData);

	// 서버측 시각적 업데이트 강제 호출
	OnRep_EquipSlots();
}


void UEquipmentComponent::server_UnEquipItem_Implementation(eItemEquipType SlotType)
{
	// 1. 해당 슬롯에 아이템이 있는지 확인
	FItemEquipSlotData* SlotData = EquipSlots.FindByKey(SlotType); // operator== 필요함. 없으면 람다식 사용

	// 람다식으로 찾는법:
	FItemEquipSlotData* FoundSlot = EquipSlots.FindByPredicate([SlotType](const FItemEquipSlotData& Data) {
		return Data.Type == SlotType;
		});

	if (!FoundSlot) return; // 장착된 게 없으면 리턴

	// 2. 인벤토리 컴포넌트 가져오기
	UInventoryComponent* Inventory = Cast<ACC_PlayerState>(GetOwner())->GetInventory();
	if (!Inventory) return;

	// 3. 인벤토리가 꽉 찼는지 확인 (선택 사항이지만 안전을 위해)
	// (TryAddItem이 알아서 처리해주긴 함)

	ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwner());
	if (nullptr == PS)
		return;

	ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(PS->GetPawn());
	if (nullptr == Character)
		return;

	UAbilityComponent* Abil = Character->GetAbilityComp();
	if (nullptr == Abil)
		return;


	//  아이템 데이터 복원
	FItemData ItemToReturn;
	ItemToReturn.TID = FoundSlot->ItemEquipData.TID;
	ItemToReturn.UID = FoundSlot->ItemEquipData.UID;
	ItemToReturn.Enchant = FoundSlot->ItemEquipData.Enchant;
	ItemToReturn.Count = 1;

	//어빌리티 삭제
	Abil->EraseAbilData(eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP, ItemToReturn.TID);


	//  인벤토리에 추가
	Inventory->TryAddItem(ItemToReturn);

	//. 장비창 목록에서 제거
	EquipSlots.RemoveAll([SlotType](const FItemEquipSlotData& Data) {
		return Data.Type == SlotType;
		});

	//. 변경 알림 (외형 업데이트 포함)
	OnRep_EquipSlots();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UEquipmentComponent, EquipSlots);
}
