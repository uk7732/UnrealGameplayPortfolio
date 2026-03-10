// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Common/GameDefines.h"
#include "Common/GameDatas.h"
#include "ActiveSpawnPoint.generated.h"

class ACC_MonsterCharacter;
class ACC_CharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, class ACC_MonsterCharacter*, DeadMonster);

/**
 *
 */
UCLASS()
class UActiveSpawnPoint : public UObject
{
	GENERATED_BODY()

public:
	UActiveSpawnPoint();

public:
	UFUNCTION()
	void RequestSpawnAction();

	UFUNCTION()
	bool InitializerActiveSpawnPoint(const FName& SpawnTID);
	bool IsIntervalTimeCheck(float IntervalTime);

	const FName& GetSpawnName();
	float GetSpawnInterval();
	bool GetIsTriggerSpawnType();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = On_RepMonsterCountReset)
	TArray<ACC_CharacterBase*> SpawnCharacters;


	eSpawnModeType Type = eSpawnModeType::NONE;
	eSpawnCharacterType CharacterType = eSpawnCharacterType::SPAWN_NONE;
	bool bIsTriggerSpawnType = false;
	//스폰 존 데이터를 담은 구조체 
	FSpawnZoneData SpawnZoneData;
private:
	
	void SinglePoint_SpawnAction();
	void RandomCircle_SpawnAction();
	void RandomRectangle_SpawnAction();
	void SpawnAndSetupMonster(const FVector& Location, const FRotator& Rotator);
	void SpawnAndSetupNPC(const FVector& Location, const FRotator& Rotation);
	bool TryGetValidSpawnLocation(const FVector& ProposedLocation, FVector& OutValidLocation);
	//스폰된 몬스터에 델리게이트 바인딩
	UFUNCTION()
	void SetupCharacter(ACC_CharacterBase* NewMonster);

	//바인딩된 함수 죽을때 호출
	UFUNCTION()
	void HandleMonsterDeath(ACC_MonsterCharacter* DeadMonster);

	//몬스터 객체 카운트 확인용
	UFUNCTION()
	void On_RepMonsterCountReset();

	UFUNCTION(Server, Reliable)
	void server_RequestSpawnAction();
	void server_RequestSpawnAction_Implementation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
