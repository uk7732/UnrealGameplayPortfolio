// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Admin4_GameInstance.generated.h"

/**
 *
 */
UCLASS()
class FRONTENDUI_API UAdmin4_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // UID를 관리하는 함수 (서버 전용)
    int64 GetNextItemUID_Server();

    // 싱글 플레이 시작
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void LaunchSingleplayer(FName LevelName);

    // 멀티플레이 호스트 (방장) 시작
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void HostMultiplayer(FName LevelName);

    // 멀티플레이 참가 (클라이언트)
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void JoinMultiplayer(FString IPAddress);

    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void NextMap(FString MapPath);

private:
    // 현재까지 생성된 가장 큰 UID를 저장하는 변수 (서버에서만 관리)
    UPROPERTY()
    int64 LastAssignedItemID = 0;


};
