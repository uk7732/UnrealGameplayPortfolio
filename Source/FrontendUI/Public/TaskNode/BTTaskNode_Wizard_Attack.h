// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Wizard_Attack.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UBTTaskNode_Wizard_Attack : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTaskNode_Wizard_Attack();
protected:
    // 태스크가 실행될 때 호출되는 핵심 함수
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    // 태스크가 종료될 때 처리를 위한 함수 (필요 시)
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:

};
