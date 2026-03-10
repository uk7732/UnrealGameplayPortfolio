// Vince Petrelli All Rights Reserved


#include "GameMode/LobbyGameModeBase.h"
#include "Instance/TableSubsystem.h"

ALobbyGameModeBase::ALobbyGameModeBase()
{


}

void ALobbyGameModeBase::InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage)
{
    Super::InitGame(MapName, Oprions, ErrorMessage);

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
