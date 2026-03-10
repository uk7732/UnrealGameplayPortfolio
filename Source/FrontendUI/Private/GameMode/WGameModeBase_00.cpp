// Vince Petrelli All Rights Reserved


#include "GameMode/WGameModeBase_00.h"
#include "Instance/TableSubsystem.h"
#include "Component/SpawnManagerComponent.h"

void AWGameModeBase_00::InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage)
{
	Super::InitGame(MapName, Oprions, ErrorMessage);
}

void AWGameModeBase_00::BeginPlay()
{
	Super::BeginPlay();

    UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
    if (nullptr == tablesystem)
        return;

    FDefineTableRow* TestBot_Spawn_TID = tablesystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::LobbySpawn);
    if (nullptr == TestBot_Spawn_TID)
        return;

    const FName SpawnGroupTID = FName(TestBot_Spawn_TID->StrVal);

    SpawnManager->InitActiveSpawnPoint(SpawnGroupTID);
}
