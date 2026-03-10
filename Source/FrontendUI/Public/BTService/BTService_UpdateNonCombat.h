// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateNonCombat.generated.h"

/**
 * 
 */
UCLASS()
class UBTService_UpdateNonCombat : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateNonCombat();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
