// Vince Petrelli All Rights Reserved


#include "TaskNode/BTTask_FindPatrolPos.h"
#include "Characters/CC_MonsterCharacter.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Controllers/CC_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h" 
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Component/SkillComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Monster || !BB) return EBTNodeResult::Failed;

    FSenseConfigData Config = Monster->GetSenseConfigData();
    FVector HomeLocation = BB->GetValueAsVector("HomeLocation");

    FNavLocation NextLocation;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys->GetRandomReachablePointInRadius(HomeLocation, Config.PatrolRadius, NextLocation))
    {
        FVector FinalPos = NextLocation.Location;
        
        FinalPos.Z = HomeLocation.Z;

        // 블랙보드에 목적지 저장
        BB->SetValueAsVector("PatrolLocation", FinalPos);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Succeeded;
}

void UBTTask_FindPatrolPos::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_FindPatrolPos::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}
