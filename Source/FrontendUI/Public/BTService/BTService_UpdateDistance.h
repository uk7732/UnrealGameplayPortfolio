// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateDistance.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UBTService_UpdateDistance : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_UpdateDistance();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp,  uint8* NodeMemory, float DeltaSeconds) override;

};
