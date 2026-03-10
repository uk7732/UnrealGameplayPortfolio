// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WGameModeBase.h"
#include "WGameModeBase_00.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API AWGameModeBase_00 : public AWGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void InitGame(const FString& MapName, const FString& Oprions, FString& ErrorMessage) override;

	virtual void BeginPlay() override;

};
