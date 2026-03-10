// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/GameDatas.h"

#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


class AItemBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FRONTENDUI_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 블루프린트에서 바인딩할 수 있는 이벤트 변수
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	// [블루프린트용 Getter] UI에서 아이템 목록을 가져가기 위함
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FItemData>& GetInventoryItems() const { return InventoryItems; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_InventoryItems, Category = "Inventory")
	TArray<FItemData> InventoryItems;

	//만약 인벤토리 최대공간 동적으로 관리할시 서버함수를 호출해서 해야함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
	int32 InventoryMaxSpace = 24;	// 인벤토리 슬롯 갯수 설정


public:
	UFUNCTION()
	void TryAddItem(const FItemData& ItemDataToAdd, AItemBase* WorldItemActor = nullptr);
	UFUNCTION()
	void TryEraseIten(const FName& ItemTID, int64 UID);
	UFUNCTION()
	void TryEraseStackItem(const FName& ItemTID, int64 UID, int32 count);
	UFUNCTION()
	void TryDropIten(const FName& ItemTID, int64 UID);
	UFUNCTION()
	void TryDropStackItem(const FName& ItemTID, int64 UID, int32 count);

	// 아이템 갯수 반환 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(FName ItemTID);

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemData GetAmmoData();

	// 인벤토리 슬롯 간 아이템 위치 교환 요청 (UI -> Server)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestSwapItems(int32 IndexA, int32 IndexB);

	// 인벤토리 정렬 요청 (UI -> Server)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestSortInventory();

	// 인벤토리 최대 크기 반환 (UI 생성용)
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetInventoryMaxSpace() const { return InventoryMaxSpace; }

	// UI에서 호출할 버리기 요청 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestDropItem(FName ItemTID, int64 ItemUID);

private:
	// 서버: 위치 교환 구현
	UFUNCTION(Server, Reliable)
	void server_SwapItems(int32 IndexA, int32 IndexB);

	// 서버: 정렬 구현
	UFUNCTION(Server, Reliable)
	void server_SortInventory();

private:
	int32 FindStackableSlotIndex(const FItemData& ItemDataToAdd, int32 MaxStackCount);
	bool IsNewSlotAvailable();


	UFUNCTION(Server, Reliable)
	void server_TryAddItem(const FItemData& ItemDataToAdd, AItemBase* WorldItemActor = nullptr);
	void server_TryAddItem_Implementation(const FItemData& ItemDataToAdd, AItemBase* WorldItemActor = nullptr);

	//====아이템 삭제===
	UFUNCTION(Server, Reliable)
	void server_TryEraseItem(const FName& ItemTID, int64 UID);
	void server_TryEraseItem_Implementation(const FName& ItemTID, int64 UID);

	UFUNCTION(Server, Reliable)
	void server_TryEraseStackItem(const FName& ItemTID, int64 UID, int32 count);
	void server_TryEraseStackItem_Implementation(const FName& ItemTID, int64 UID, int32 count);
	//===================

	//아이템 바닥에 버림=====
	UFUNCTION(Server, Reliable)
	void server_TryDropItem(const FName& ItemTID, int64 UID);
	void server_TryDropItem_Implementation(const FName& ItemTID, int64 UID);


	UFUNCTION(Server, Reliable)
	void server_TryDropStackItem(const FName& ItemTID, int64 UID, int32 count);
	void server_TryDropStackItem_Implementation(const FName& ItemTID, int64 UID, int32 count);

	// 아이템 배열이 갱신되면 호출될 함수
	UFUNCTION()
	void OnRep_InventoryItems();


public:
	// UI(위젯)에서 클릭 시 호출하는 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Client_RequestUseItem(int64 ItemUID);

private:
	// 서버: 아이템 사용 처리
	UFUNCTION(Server, Reliable)
	void server_UseItem(int64 ItemUID);
	void server_UseItem_Implementation(int64 ItemUID);
	//=======================

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
