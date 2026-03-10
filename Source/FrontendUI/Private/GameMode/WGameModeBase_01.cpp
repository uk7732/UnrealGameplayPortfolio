// Vince Petrelli All Rights Reserved


#include "GameMode/WGameModeBase_01.h"
#include "Instance/TableSubsystem.h"
#include "Component/SpawnManagerComponent.h"

void AWGameModeBase_01::InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage)
{
	Super::InitGame(MapName, Oprions, ErrorMessage);
}

void AWGameModeBase_01::BeginPlay()
{
	Super::BeginPlay();

    InitRoomNumCount(3);

    UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
    if (nullptr == tablesystem)
        return;

    FDefineTableRow* MonsterSpawn_01_TID = tablesystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::MonsterSpawn_01_TID);
    if (nullptr == MonsterSpawn_01_TID)
        return;

    const FName SpawnGroupTID = FName(MonsterSpawn_01_TID->StrVal);

    SpawnManager->InitActiveSpawnPoint(SpawnGroupTID);

}

void AWGameModeBase_01::EraseMonsterCount(int32 RoomNum)
{
    Super::EraseMonsterCount(RoomNum);
}
