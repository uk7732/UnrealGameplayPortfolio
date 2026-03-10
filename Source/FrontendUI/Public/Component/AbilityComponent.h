// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/GameDatas.h"
#include "AbilityComponent.generated.h"

// 체력바를 위한 델리케이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTENDUI_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();
		
protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP;

	UPROPERTY(ReplicatedUsing = OnRep_MP)
	float CurrentMP;

	//초당 마나 회복량
	UPROPERTY(Replicated)
	float ManaPerSecond;

	float PrevHP;
	float PrevMP;
	
	void UpdateCurrentHP_And_MP(float PrevMaxHP, float PrevMaxMP, float PrevManaPerSecond);
public:
	const float GetHP() { return CurrentHP; }
	const float GetMP() { return CurrentMP; }
	void ApplyHP(float hp) { server_ApplyHP(hp); }
	void ApplyMP(float mp) { server_ApplyMP(mp); }
	
	
protected:
	// 캐릭터의 순수 기본 능력치 목록 (데이터 테이블에서 읽어옴)
	UPROPERTY(ReplicatedUsing = OnRep_DefaultAbils)
	TArray<FAbilityData> DefaultAbils;

	UPROPERTY(ReplicatedUsing = OnRep_ItemEquipAbils)
	TArray<FAbilityData> ItemEquipAbils;

	UPROPERTY(ReplicatedUsing = OnRep_ItemOwnedAbils)
	TArray<FAbilityData> OwnedAbils;

	UPROPERTY(ReplicatedUsing = OnRep_BuffAbils)
	TArray<FAbilityData> BuffAbils;

	UPROPERTY(ReplicatedUsing = OnRep_DeBuffAbils)
	TArray<FAbilityData> DeBuffAbils;

	// 아이템, 버프 등 모든 효과가 합산된 최종 능력치 목록
	UPROPERTY(ReplicatedUsing = OnRep_TotalAbils)
	TArray<FAbilityData> TotalAbils;
	
	FTimerHandle MPRegenTimer;
	FTimerHandle BuffTimerHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	//디폴드 값은 Type가 중복되면 안됨 덮어쓰거나 상수자체를 추가제거하거나 둘중 하나
	void SetDefaultAbil(const TArray<FAbilityData>& abil);     //<-- TArray 값 전체가 들어오는 함수 (실행되면 DefaultAbils.Empty() 컨테이너 전부 비움)
	void ApplyDefaultAbil(const FAbilityData& abil);           //<-- 각 카테고리의 값 수치 조정가능
	void AddAbilData(eAbilCategoryType category, const TArray<FAbilityData>& abil);
	void AddAbilData(eAbilCategoryType category, const FAbilityData& abil);
	void EraseAbilData(eAbilCategoryType category, FName TID);

	//Default , Total값만 조회가능
	const FAbilityData* GetAbil(eAbilCategoryType category, eAbilType type);

private:

	const FAbilityData* GetDefaultAbil(eAbilType type);
	const FAbilityData* GetTotalAbil(eAbilType type);

	void AddEquipAbil(const TArray<FAbilityData>& abil);
	void AddEquipAbil(const FAbilityData& abil);
	void EraseEquipAbil(FName TID);
	
	void AddOwnedAbil(const TArray<FAbilityData>& abil);
	void AddOwnedAbil(const FAbilityData& abil);
	void EraseOwnedAbil(FName TID);
	
	void AddBuffAbil(const TArray<FAbilityData>& abil);
	void AddBuffAbil(const FAbilityData& abil);
	void EraseBuffAbil(FName TID);

	void AddDeBuffAbil(const TArray<FAbilityData>& abil);
	void AddDeBuffAbil(const FAbilityData& abil);
	void EraseDeBuffAbil(FName TID);

	void RefreshTotalAbil();

public:
	// 블루프린트에서 호출할 수 있도록 설정
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetHPRatio();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetMPRatio();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetMaxHP();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetMaxMP();

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnHPChanged OnHPChanged;

private:
	UFUNCTION()
	void OnRep_ItemEquipAbils();

	UFUNCTION()
	void OnRep_ItemOwnedAbils();

	UFUNCTION()
	void OnRep_DefaultAbils();

	UFUNCTION()
	void OnRep_BuffAbils();

	UFUNCTION()
	void OnRep_DeBuffAbils();

	UFUNCTION()
	void OnRep_TotalAbils();

	UFUNCTION()
	void OnRep_CurrentHP();
	UFUNCTION()
	void OnRep_MP();

public:
	//어빌리티 데이터 다수 입력
	UFUNCTION(Server, Reliable)
	void server_AddAbilData(eAbilCategoryType category, const TArray<FAbilityData>& AbilToApply);
	void server_AddAbilData_Implementation(eAbilCategoryType category, const TArray<FAbilityData>& AbilToApply);

	//어빌리티 데이터 단일 입력
	UFUNCTION(Server, Reliable)
	void server_AddAbilDataSingle(eAbilCategoryType category, const FAbilityData AbilToApply);
	void server_AddAbilDataSingle_Implementation(eAbilCategoryType category, const FAbilityData AbilToApply);

	//어빌리티 데이터 삭제
	UFUNCTION(Server, Reliable)
	void server_EraseAbilData(eAbilCategoryType category, FName TID);
	void server_EraseAbilData_Implementation(eAbilCategoryType category, FName TID);

private:
	UFUNCTION(Server, Reliable)
	void server_RegenMP();
	void server_RegenMP_Implementation();

	UFUNCTION(Server, Reliable)
	void server_UpdateBuffs_DeBuffs();
	void server_UpdateBuffs_DeBuffs_Implementation();

	//디폴트 값 추가, 갱신
	UFUNCTION(Server, Reliable)
	void server_SetDefaultAbil(const TArray<FAbilityData>& abils);
	void server_SetDefaultAbil_Implementation(const TArray<FAbilityData>& abils);

	//디폴드 값 제거 
	UFUNCTION(Server, Reliable)
	void server_ApplydefaultAbil(const FAbilityData& abil);
	void server_ApplydefaultAbil_Implementation(const FAbilityData& abil);
	

	//현재 HP 증가 감소 로직
	UFUNCTION(Server, Reliable)
	void server_ApplyHP(float Amount);
	void server_ApplyHP_Implementation(float Amount);

	//현재 MP 증가 감소 로직
	UFUNCTION(Server, Reliable)
	void server_ApplyMP(float Amount);
	void server_ApplyMP_Implementation(float Amount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
