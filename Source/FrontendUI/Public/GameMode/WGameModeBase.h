// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WGameModeBase.generated.h"



/**
 * 
 */
UCLASS()
class FRONTENDUI_API AWGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWGameModeBase();



protected:
	// 게임이 처음 시작될 때 호출되는 함수
	virtual void InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage) override;

	virtual void BeginPlay() override;

	class USpawnManagerComponent* SpawnManager;

public:


	virtual void EraseMonsterCount(int32 RoomNum);

	virtual void CheckLastRoomClearAndTravel();

	void AddMonsterCount(int32 RoomNum);
	
	void InitRoomNumCount(int32 RoomCount);
	
private:
	int32 LastRoomNum = -1;
};
