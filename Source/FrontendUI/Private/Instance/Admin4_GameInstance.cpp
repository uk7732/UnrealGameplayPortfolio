// Vince Petrelli All Rights Reserved


#include "Instance/Admin4_GameInstance.h"
#include "Kismet/GameplayStatics.h" 


int64 UAdmin4_GameInstance::GetNextItemUID_Server()
{
    return LastAssignedItemID++;
}

void UAdmin4_GameInstance::LaunchSingleplayer(FName LevelName)
{
    // 단순 레벨 이동 (옵션 없음)
    UGameplayStatics::OpenLevel(this, LevelName);

    UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 싱글플레이 시작: %s"), *LevelName.ToString());
}

void UAdmin4_GameInstance::HostMultiplayer(FName LevelName)
{
    // 리슨 서버로 레벨 열기 (?listen 옵션 추가)
    // 이렇게 열면 이 PC가 서버이자 플레이어가 됩니다.
    UGameplayStatics::OpenLevel(this, LevelName, true, TEXT("listen"));

    UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 멀티플레이 호스트 시작 (Listen Server): %s"), *LevelName.ToString());
}

void UAdmin4_GameInstance::JoinMultiplayer(FString IPAddress)
{
    if (IPAddress.IsEmpty())
    {
        IPAddress = TEXT("127.0.0.1"); // 기본값 (로컬호스트)
    }

    // IP 주소로 레벨 열기 -> 해당 주소의 서버로 접속 시도
    UGameplayStatics::OpenLevel(this, *IPAddress);

    UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 멀티플레이 참가 시도: %s"), *IPAddress);
}

void UAdmin4_GameInstance::NextMap(FString MapPath)
{
    UWorld* World = GetWorld();
    if (World)
    {
        // [중요] OpenLevel 대신 ServerTravel을 사용합니다.
        // URL 옵션(?listen)을 포함하여 목적지를 설정합니다.
        FString TravelURL = MapPath + TEXT("?listen");

        // 모든 연결된 클라이언트를 데리고 함께 맵을 이동합니다.
        World->ServerTravel(TravelURL);
    }
}
