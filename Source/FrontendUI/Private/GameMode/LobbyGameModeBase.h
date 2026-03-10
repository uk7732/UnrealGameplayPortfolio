// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALobbyGameModeBase();

protected:
	// 게임이 처음 시작될 때 호출되는 함수
	virtual void InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage) override;

};
