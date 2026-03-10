// Vince Petrelli All Rights Reserved


#include "Component/InventoryComponent.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Actor/Item/ItemBase.h"
#include "Instance/Admin4_GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Component/EquipmentComponent.h"
#include "Component/AbilityComponent.h"
#include "GameMode/CC_PlayerState.h"

#include "Characters/CC_CharacterBase.h"

#include "FrontendDebugHelper.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicatedByDefault(true);

    // ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
    // 
      // 서버 권한이 있을 때만 데이터 초기화
    if (GetOwner()->HasAuthority())
    {
        // FItemData 생성자는 기본적으로 TID=NAME_None으로 초기화된다고 가정
        InventoryItems.Init(FItemData(), InventoryMaxSpace);    
        UAdmin4_GameInstance* Instance = Cast<UAdmin4_GameInstance>(GetOwner()->GetGameInstance());
        //if (nullptr == Instance) return;
        if (Instance)
        {
            // 탄약 데이터 생성 및 10발 지급
            FItemData AmmoData;
            AmmoData.TID = FName("Ammo"); // 데이터 테이블의 TID와 일치해야 함
            AmmoData.Count = 200;          // 요구사항: 10발 지급
            
            if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetWorld()->GetGameInstance()))
            {
                AmmoData.UID = GI->GetNextItemUID_Server();
            }
            
            // 인벤토리에 바로 추가 (TryAddItem을 쓰면 바닥 드랍 로직 등을 타므로 직접 추가하거나 함수 활용)
            // 여기서는 배열에 직접 넣거나, 안전하게 구현된 TryAddItem을 호출하는 것이 좋습니다.
            // 기존 코드 패턴대로 배열에 직접 넣습니다.     
            
            InventoryItems[0] = AmmoData;

        }
    }
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UInventoryComponent::TryAddItem(const FItemData& ItemDataToAdd, AItemBase* WorldItemActor)
{
    server_TryAddItem(ItemDataToAdd, WorldItemActor);
}

void UInventoryComponent::TryEraseIten(const FName& ItemTID, int64 UID)
{
    server_TryEraseItem(ItemTID, UID);
}

void UInventoryComponent::TryEraseStackItem(const FName& ItemTID, int64 UID, int32 count)
{
    server_TryEraseStackItem(ItemTID, UID, count);
}

void UInventoryComponent::TryDropIten(const FName& ItemTID, int64 UID)
{
    server_TryDropItem(ItemTID, UID);
}

void UInventoryComponent::TryDropStackItem(const FName& ItemTID, int64 UID, int32 count)
{
    server_TryDropStackItem(ItemTID, UID, count);
}

int32 UInventoryComponent::GetItemCount(FName ItemTID)
{
    int32 TotalCount = 0;

    // 인벤토리 배열을 순회하며 개수 합산
    for (const FItemData& Item : InventoryItems)
    {
        if (Item.TID == ItemTID)
        {
            TotalCount += Item.Count;
        }
    }

    return TotalCount;
}

FItemData UInventoryComponent::GetAmmoData()
{
    for (auto& it : InventoryItems)
    {
        if (it.TID == FName("Ammo"))
        {
            return it;
        }
    }

    return FItemData();
}

void UInventoryComponent::RequestSwapItems(int32 IndexA, int32 IndexB)
{
    server_SwapItems(IndexA, IndexB);
}

void UInventoryComponent::RequestSortInventory()
{
    server_SortInventory();
}

void UInventoryComponent::RequestDropItem(FName ItemTID, int64 ItemUID)
{
    // 서버 RPC 호출
    server_TryDropItem(ItemTID, ItemUID);
}

void UInventoryComponent::server_SwapItems_Implementation(int32 IndexA, int32 IndexB)
{
    // 인덱스 유효성 검사
    if (InventoryItems.IsValidIndex(IndexA) && InventoryItems.IsValidIndex(IndexB))
    {
        // 두 슬롯의 데이터 교체
        InventoryItems.Swap(IndexA, IndexB);

        // 변경 사항 알림
        OnRep_InventoryItems();
    }
}

void UInventoryComponent::server_SortInventory_Implementation()
{
    // 람다식을 이용한 정렬
    // 규칙: 아이템이 있는 것이 앞으로 오고, 이름 순으로 정렬
    InventoryItems.Sort([](const FItemData& A, const FItemData& B)
        {
            bool bAValid = !A.TID.IsNone();
            bool bBValid = !B.TID.IsNone();

            // A만 있고 B는 없으면 -> A가 앞 (True)
            if (bAValid && !bBValid) return true;
            // B만 있고 A는 없으면 -> B가 앞 (False)
            if (!bAValid && bBValid) return false;
            // 둘 다 없으면 -> 순서 상관 없음 (False)
            if (!bAValid && !bBValid) return false;

            // 둘 다 있으면 -> TID(이름) 기준 오름차순 정렬
            return A.TID.ToString() < B.TID.ToString();
        });

    // 변경 사항 알림
    OnRep_InventoryItems();
}

int32 UInventoryComponent::FindStackableSlotIndex(const FItemData& ItemDataToAdd, int32 MaxStackCount)
{
    for (int32 i = 0; i < InventoryItems.Num(); ++i)
    {
        // TID가 같고, 현재 Count가 최대 스택 수량 미만인 경우
        if (InventoryItems[i].TID == ItemDataToAdd.TID && InventoryItems[i].Count < MaxStackCount)
        {
            // 2. 현재 슬롯에 추가할 공간이 남아있는가? (Count < MaxStackCount)
            if (InventoryItems[i].Count < MaxStackCount)
            {
                return i; // 부분 스택 가능한 슬롯 인덱스 반환
            }
        }
    }
    return INDEX_NONE; // (-1) 합쳐질 슬롯을 찾지 못함
}

bool UInventoryComponent::IsNewSlotAvailable()
{
    return false;
}

void UInventoryComponent::server_TryAddItem_Implementation(const FItemData& ItemDataToAdd, AItemBase* WorldItemActor)
{
    UTableSubsystem* Tablesub = UTableSubsystem::Get(this);
    if (nullptr == Tablesub)
        return;

    FItemTableRow* ItemTable = Tablesub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemDataToAdd.TID);
    if (nullptr == ItemTable)
        return;

    int32 FoundIndex = -1;

    eItemType Type = ItemTable->Type;
    int32 MaxStackCount = ItemTable->MaxStackCount; // 테이블에서 MaxStackCount를 가져옴 (가정)

    // 획득해야 할 잔여 수량
    int32 RemainderCount = ItemDataToAdd.Count;

    // 획득 성공 여부 플래그 (하나라도 획득했으면 성공)
    bool bOverallSuccess = false;

    // 1. 기존 슬롯에 스택 합치기 시도
    if (ItemTable->Type == eItemType::CONSUME || ItemTable->Type == eItemType::COMPONENT)
    {
        for (int32 i = 0; i < InventoryItems.Num() && RemainderCount > 0; ++i)
        {
            if (InventoryItems[i].TID == ItemDataToAdd.TID && InventoryItems[i].Count < MaxStackCount)
            {
                int32 AddedCount = FMath::Min(RemainderCount, MaxStackCount - InventoryItems[i].Count);
                InventoryItems[i].Count += AddedCount;
                RemainderCount -= AddedCount;
                bOverallSuccess = true;
            }
        }
    }

    // 2. 남은 수량이 있다면 빈 슬롯에 새로 추가
    for (int32 i = 0; i < InventoryItems.Num() && RemainderCount > 0; ++i)
    {
        if (InventoryItems[i].TID.IsNone()) // 빈 슬롯 발견
        {
            FItemData NewData = ItemDataToAdd;
            NewData.Count = FMath::Min(RemainderCount, MaxStackCount);

            // UID 발급 (스택 불가능하거나 새 슬롯일 때)
            if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetWorld()->GetGameInstance()))
            {
                NewData.UID = GI->GetNextItemUID_Server();
            }

            InventoryItems[i] = NewData;
            RemainderCount -= NewData.Count;
            bOverallSuccess = true;

            // 스택 불가능 아이템이면 하나 넣고 끝
            if (ItemTable->Type != eItemType::CONSUME && ItemTable->Type != eItemType::COMPONENT)
                break;
        }
    }

    // 3. 최종 결과 처리
    if (bOverallSuccess)
    {
        // 획득 성공! 바닥 액터 파괴
        if (WorldItemActor)
        {
            WorldItemActor->Destroy();
            OnRep_InventoryItems();
        }

        OnRep_InventoryItems();
        // 서버(Host) 플레이어의 UI 갱신을 위해 직접 호출
        if (OnInventoryUpdated.IsBound())
        {
            OnInventoryUpdated.Broadcast();
        }
    }
    else
    {
        // 획득 실패 (인벤토리 가득 참)
        // 바닥 아이템은 파괴하지 않고 유지
    }
}

void UInventoryComponent::server_TryEraseItem_Implementation(const FName& ItemTID, int64 UID)
{
    //int64 uid = UID;
    //InventoryItems.RemoveAll([uid](const FItemData& Data)
    //    {
    //        return uid == Data.UID;
    //    });

    for (int32 i = 0; i < InventoryItems.Num(); ++i)
    {
        if (InventoryItems[i].UID == UID)
        {
            InventoryItems[i] = FItemData(); // 빈 슬롯으로 초기화
            break;
        }
    }

    OnRep_InventoryItems();
}

void UInventoryComponent::server_TryEraseStackItem_Implementation(const FName& ItemTID, int64 UID, int32 count)
{
    //for (auto& it : InventoryItems)
    //{
    //    if (it.TID == ItemTID && it.UID == UID)
    //    {
    //        if (it.Count == count)
    //        {
    //            int64 uid = UID;
    //            InventoryItems.RemoveAll([uid](const FItemData& Data)
    //                {
    //                    return uid == Data.UID;
    //                });
    //        }
    //        else
    //        {
    //            it.Count -= count;
    //            return;
    //        }
    //    }
    //}

    for (int32 i = 0; i < InventoryItems.Num(); ++i)
    {
        // 참조(&)로 가져와서 바로 수정
        FItemData& Item = InventoryItems[i];

        if (Item.TID == ItemTID && Item.UID == UID)
        {
            if (Item.Count <= count)
            {
                // [수정] 개수가 0 이하가 되면 배열 삭제가 아니라 '빈 데이터'로 변경
                InventoryItems[i] = FItemData();
            }
            else
            {
                // 개수만 차감
                Item.Count -= count;
            }

            // 찾았으니 루프 종료 및 갱신
            OnRep_InventoryItems();
            return;
        }
    }
}

void UInventoryComponent::server_TryDropItem_Implementation(const FName& ItemTID, int64 UID)
{
    FItemData DroppedItemData;
    int64 uid = UID;
    int32 FoundIndex = -1;

    for (int32 i = 0; i < InventoryItems.Num(); ++i)
    {
        if (InventoryItems[i].UID == UID)
        {
            // 슬롯 비우기 전 아이템 정보 복사
            DroppedItemData = InventoryItems[i];
         
            // 데이터를 초기화 (TID = None, Count = 0 등), 인벤토리 슬롯 비우기
            InventoryItems[i] = FItemData();

            FoundIndex = i;

            break;
        }
    }

    // 인벤토리에서 해당 아이템을 못 찾았으면 함수를 종료합니다.
    if (FoundIndex == -1) return;

    ACC_PlayerState* player = GetOwner<ACC_PlayerState>();
    if (!player) return;

    // 2. PlayerState를 통해 현재 빙의된 Pawn(캐릭터)을 가져옵니다.
    ACC_PlayerCharacter* PlayerCharacter = Cast<ACC_PlayerCharacter>(player->GetPawn());
    if (!PlayerCharacter) return;

    UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
    if (nullptr == Tablesystem)
        return;

    FItemTableRow* ItemTable = Tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemTID);
    if (nullptr == ItemTable)
        return;

    // 2. FActorSpawnParameters 구조체 생성 및 설정
    FActorSpawnParameters SpawnParams;
       
    //FVector SpawnLocation = player->GetActorLocation();
    FVector SpawnLocation = PlayerCharacter->GetActorLocation() + (PlayerCharacter->GetActorForwardVector() * 100.0f); // 약간 앞에 스폰
    FRotator SpawnRotation = FRotator::ZeroRotator;

    SpawnParams.Owner = PlayerCharacter; // Player를 소유자로 설정 (선택 사항이지만 권장)
    SpawnParams.Instigator = PlayerCharacter; // Player를 유발자로 설정 (선택 사항)
    
    //스폰 지역에 충돌시 주변에 빈곳에 아무곳이나 무조건 스폰되게 하는 enum값이라함
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 월드에 아이템 액터 생성
    AItemBase* item = Cast<AItemBase>(GetWorld()->SpawnActor(ItemTable->ItemClass, &SpawnLocation, &SpawnRotation, SpawnParams));
    item->InitializedItemData(ItemTID, UID);

    if (item)
    {
        // 5. [핵심] 생성된 액터에게 '복사해 둔' 아이템 정보(TID, UID, Count)를 전달하여 초기화합니다.
        item->InitializedItemData(DroppedItemData.TID, DroppedItemData.UID, DroppedItemData.Count);
        Debug::Print(TEXT("드랍 아이템 액터 생성"), -1, FColor::Green);
    }

    // 6. 인벤토리 배열이 변경되었으므로 모든 클라이언트에게 변경 사항을 알립니다.
    OnRep_InventoryItems();
}

void UInventoryComponent::server_TryDropStackItem_Implementation(const FName& ItemTID, int64 UID, int32 count)
{
    ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetOwner()->GetOwner());
    if (nullptr == player)
        return;

    UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
    if (nullptr == Tablesystem)
        return;

    FItemTableRow* ItemTable = Tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, ItemTID);
    if (nullptr == ItemTable)
        return;

    // 2. FActorSpawnParameters 구조체 생성 및 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = player; // Player를 소유자로 설정 (선택 사항이지만 권장)
    SpawnParams.Instigator = player; // Player를 유발자로 설정 (선택 사항)
    //스폰 지역에 충돌시 주변에 빈곳에 아무곳이나 무조건 스폰되게 하는 enum값이라함
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    FVector SpawnLocation = player->GetActorLocation();
    FRotator SpawnRotator = FRotator::ZeroRotator;
    AItemBase* item = Cast<AItemBase>(GetWorld()->SpawnActor(ItemTable->ItemClass, &SpawnLocation, &SpawnRotator, SpawnParams));
    item->InitializedItemData(ItemTID, UID, count);


    FItemData* itemdata = InventoryItems.FindByKey<int64>(UID);
    if (nullptr == itemdata)
        return;

    if (itemdata->Count == count)
    {
        int64 uid = UID;
        InventoryItems.RemoveAll([uid](FItemData& Data)
            {
                return Data.UID == uid;
            });
    }
    else
    {
        itemdata->Count -= count;
    }
}

// 서버로부터 아이템 목록이 갱신되면 자동으로 호출 
void UInventoryComponent::OnRep_InventoryItems()
{
    // UI(위젯)에게 "인벤토리 내용 바꼈으니까 다시 그려!"라고 방송(Broadcast)함
    if (OnInventoryUpdated.IsBound())
    {
        OnInventoryUpdated.Broadcast();
    }
}

void UInventoryComponent::Client_RequestUseItem(int64 ItemUID)
{
    server_UseItem(ItemUID);
}

void UInventoryComponent::server_UseItem_Implementation(int64 ItemUID)
{
    // A. 아이템 존재 여부 확인 (재사용: InventoryItems 배열)
    FItemData* FoundItem = InventoryItems.FindByKey(ItemUID);
    if (!FoundItem)
    {
        UE_LOG(LogTemp, Error, TEXT("[Server] 실패: 인벤토리에서 아이템(UID: %lld)을 찾을 수 없습니다."), ItemUID);
        return;
    }

    // B. 테이블 데이터 조회 (재사용: TableSubsystem)
    UTableSubsystem* TableSub = UTableSubsystem::Get(this);
    if (!TableSub) return;

    FItemTableRow* ItemRow = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, FoundItem->TID);
    if (!ItemRow)
    {
        UE_LOG(LogTemp, Error, TEXT("[Server] 실패: 아이템 테이블에서 '%s'를 찾을 수 없습니다."), *FoundItem->TID.ToString());
        return;
    } 
    
    // === 로그 출력: 아이템의 정체 확인 ===
    FString TypeEnumStr = UEnum::GetValueAsString(ItemRow->Type);
    UE_LOG(LogTemp, Warning, TEXT("[Server] 사용 시도: %s (Type: %s)"), *FoundItem->TID.ToString(), *TypeEnumStr);

    // C. 컴포넌트 가져오기
    ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwner());
    if (!PS) return;
    ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(PS->GetPawn());

    // === 분기 처리 ===

    // CASE 1: 장비 아이템 (무기, 방어구)
    if (ItemRow->Type == eItemType::WEAPON_R || ItemRow->Type == eItemType::WEAPON_L || ItemRow->Type == eItemType::SHIELD || ItemRow->Type == eItemType::DEFENCE || ItemRow->Type == eItemType::HELMET)
    {
        if (UEquipmentComponent* EquipComp = PS->GetEquipment())
        {
            // 테이블의 장착 타입에 따라 슬롯 결정
            eItemEquipType TargetSlot = eItemEquipType::NONE; // 기본값
            // (필요 시 ItemRow 데이터에 EquipType 추가하거나 Type으로 스위칭)

            switch (ItemRow->Type)
            {
            case eItemType::WEAPON_R: TargetSlot = eItemEquipType::WEAPON_R; break;
            case eItemType::WEAPON_L: TargetSlot = eItemEquipType::WEAPON_L; break;
            case eItemType::SHIELD: TargetSlot = eItemEquipType::SHIELD; break;
            case eItemType::DEFENCE: TargetSlot = eItemEquipType::ARMOR; break;
            case eItemType::HELMET: TargetSlot = eItemEquipType::HELMET; break;

            default: break;
            }

            // [재사용] EquipmentComponent의 SetEquipSlotItem이 
            // "인벤토리 삭제 -> 장착 -> 기존템 인벤토리 추가"를 모두 수행하므로 그냥 호출만 하면 됨.
            EquipComp->SetEquipSlotItem(TargetSlot, *FoundItem);
        }
    }
    // CASE 2: 소모품 (포션)
    else if (ItemRow->Type == eItemType::CONSUME)
    {
        if (Character && Character->GetAbilityComp())
        {
            // [재사용] AbilityGroup 테이블 조회
            if (!ItemRow->AbilGroupTID.IsNone())
            {
                FAbilGroupTableRow* AbilGroup = TableSub->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, ItemRow->AbilGroupTID);
                if (AbilGroup)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Server] 성공: AbilGroup '%s' 찾음. 효과 적용 시작."), *ItemRow->AbilGroupTID.ToString());

                    // [재사용] AbilityComponent 기능 활용
                    for (const FAbilityData& Abil : AbilGroup->Abils)
                    {
                        if (Abil.Type == eAbilType::ABIL_TYPE_MAX_HP)
                        {
                            Character->ApplyHP(Abil.Value);
                            UE_LOG(LogTemp, Log, TEXT("[Server] HP 회복 적용: %.1f"), Abil.Value);
                        }
                        else if (Abil.Type == eAbilType::ABIL_TYPE_MAX_MP) Character->ApplyMP(Abil.Value);
                        else Character->AddAbilData(eAbilCategoryType::ABIL_CATEGORY_BUFF, Abil);
                    }

                    // [재사용] 효과 적용 후 아이템 1개 삭제 (UI 자동 갱신됨)
                    server_TryEraseStackItem(FoundItem->TID, FoundItem->UID, 1);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("[Server] 실패: AbilGroupTable에서 '%s'를 찾을 수 없습니다! 오타를 확인하세요."), *ItemRow->AbilGroupTID.ToString());
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[Server] 실패: 아이템 테이블에 AbilGroupTID가 비어있습니다 (None)."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Server] 실패: 이 아이템의 타입은 CONSUME이 아닙니다! 현재 타입: %s"), *TypeEnumStr);
    }
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    DOREPLIFETIME(UInventoryComponent, InventoryItems);
    DOREPLIFETIME(UInventoryComponent, InventoryMaxSpace);

}
