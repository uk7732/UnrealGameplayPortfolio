// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameMode/CC_GameStateBase.h"
#include "CC_GameStateBase_00.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ACC_GameStateBase_00 : public ACC_GameStateBase
{
	GENERATED_BODY()
	
public:
	ACC_GameStateBase_00();

protected:

	virtual void BeginPlay() override;
};
