// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/GameDatas.h"

#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FRONTENDUI_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEquipSlotItem(eItemEquipType Type, const FItemData& ItemData);

	// UI 갱신용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipmentUpdated OnEquipmentUpdated;

	// UI에서 장착된 아이템 목록을 읽어가기 위한 함수
	const TArray<FItemEquipSlotData>& GetEquipSlots() const { return EquipSlots; }

	UFUNCTION()
	void OnRep_EquipSlots(); // 선언 추가

	// 장비 해제 요청 함수 (UI -> Server)
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RequestUnEquipItem(eItemEquipType SlotType);


protected:
	// ReplicatedUsing 키워드 확인!	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_EquipSlots, Category = "Equipment")
	TArray<FItemEquipSlotData> EquipSlots;


private:
	UFUNCTION(Server, Reliable)
	void server_SetEquipSlotItem(eItemEquipType Type, const FItemData& ItemData);
	void server_SetEquipSlotItem_Implementation(eItemEquipType Type, const FItemData& ItemData);


	// 장비 해제 서버 구현
	UFUNCTION(Server, Reliable)
	void server_UnEquipItem(eItemEquipType SlotType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
