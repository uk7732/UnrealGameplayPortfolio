// Vince Petrelli All Rights Reserved


#include "GameMode/WGameModeBase.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Characters/CC_CharacterBase.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Controllers/CC_PlayerController.h"
#include "FrontendDebugHelper.h"
#include "DrawDebugHelpers.h"
#include "GameMode/CC_GameStateBase.h"
#include "Instance/Admin4_GameInstance.h"
#include "Component/SpawnManagerComponent.h"

AWGameModeBase::AWGameModeBase()
{
    // 블루프린트 클래스를 찾기 위한 경로 설정
    SpawnManager = CreateDefaultSubobject<USpawnManagerComponent>(TEXT("SpawnManager"));
}

void AWGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    
    // [추적 로그 1]
    UE_LOG(LogTemp, Error, TEXT("--- 1. WGameModeBase::InitGame 실행됨 ---"));
    if (DefaultPawnClass)
    {
        // [추적 로그 2]
        UE_LOG(LogTemp, Error, TEXT("--- 2. 스폰할 클래스: %s ---"), *DefaultPawnClass->GetName());
    }
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (tableSubsystem)
    {
        FDefineTableRow* defineRow = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::MaxProb);
        if (defineRow)
        {
            // 성공했을 때의 로그
            UE_LOG(LogTemp, Log, TEXT("[성공] MaxProb 행을 찾았습니다. 값: %d"), defineRow->IntVal);
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("[성공] MaxProb 행을 찾았습니다. 값: %d"), defineRow->IntVal));
        }
        else
        {
            // 실패했을 때의 로그
            UE_LOG(LogTemp, Warning, TEXT("[실패] MaxProb 행을 찾을 수 없습니다. DT_TableLoad와 DT_Define 에셋을 확인하세요."));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[실패] MaxProb 행을 찾을 수 없습니다."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[치명적 오류] TableSubsystem을 가져올 수 없습니다."));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[치명적 오류] TableSubsystem을 가져올 수 없습니다."));
    }
}

void AWGameModeBase::BeginPlay()
{
    Super::BeginPlay();
}



void AWGameModeBase::AddMonsterCount(int32 RoomNum)
{
    if (!HasAuthority())
        return;

    ACC_GameStateBase* GS = GetGameState<ACC_GameStateBase>();
    if (nullptr == GS)
        return;

    if (GS->RoomData.IsValidIndex(RoomNum))
    {
        GS->RoomData[RoomNum].MonsterCount++;
    }
}

void AWGameModeBase::EraseMonsterCount(int32 RoomNum)
{
    if (!HasAuthority())
        return;
    ACC_GameStateBase* GS = GetGameState<ACC_GameStateBase>();
    if (!GS || !GS->RoomData.IsValidIndex(RoomNum))
        return;

    GS->RoomData[RoomNum].MonsterCount--;

    if (GS->RoomData[RoomNum].MonsterCount <= 0)
    {
        if (GS->IsOpenRoom.IsValidIndex(RoomNum))
        {
            GS->IsOpenRoom[RoomNum] = true;
            UE_LOG(LogTemp, Log, TEXT("Room %d Cleared!"), RoomNum);
            GS->OnRep_IsOpenRoom();
        }

        if (RoomNum == LastRoomNum)
        {
            CheckLastRoomClearAndTravel();
        }
    }

}

void AWGameModeBase::InitRoomNumCount(int32 RoomCount)
{
    if (!HasAuthority())
        return;
    ACC_GameStateBase* GS = GetGameState<ACC_GameStateBase>();
    if (nullptr == GS)
        return;

    LastRoomNum = RoomCount;

    for (int i = 0; i <= LastRoomNum; ++i)
    {
        GS->RoomData.Add(FRoomData(i, 0));
        GS->IsOpenRoom.Add(false);
        GS->PrevOpenRoom.Add(false);
    }
}

void AWGameModeBase::CheckLastRoomClearAndTravel()
{
    if (!HasAuthority())
        return;

    UAdmin4_GameInstance* GameInstance = Cast<UAdmin4_GameInstance>(GetGameInstance());
    ACC_GameStateBase* GS = GetGameState<ACC_GameStateBase>();
    if (nullptr == GS)
        return;


    if (GameInstance)
    {
        UTableSubsystem* TableSystem = UTableSubsystem::Get(this);
        if (nullptr == TableSystem)
            return;

        FMapTableRow* MapTable = TableSystem->FindTableRow<FMapTableRow>(TABLE_NAME::MAP, FName("Lobby"));
        if (nullptr == MapTable)
            return;

        GS->IsOpenRoom[LastRoomNum] = false;

        FString LobbyPath = MapTable->MapPath;
        GameInstance->NextMap(LobbyPath);

        UE_LOG(LogTemp, Warning, TEXT("[GameState] 모든 몬스터 처치 완료! 로비로 이동합니다."));
    }
}