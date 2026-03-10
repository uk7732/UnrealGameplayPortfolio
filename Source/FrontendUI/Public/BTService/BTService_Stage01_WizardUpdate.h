// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Stage01_WizardUpdate.generated.h"

/**
 * 
 */
UCLASS()
class UBTService_Stage01_WizardUpdate : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Stage01_WizardUpdate();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
