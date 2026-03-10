// Vince Petrelli All Rights Reserved


#include "BTService/BTService_UpdateNonCombat.h"
#include "Characters/CC_MonsterCharacter.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Controllers/CC_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h" 
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Component/SkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_UpdateNonCombat::UBTService_UpdateNonCombat()
{
    NodeName = TEXT("Update NonCombat");

    bNotifyTick = true;
}

void UBTService_UpdateNonCombat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Monster)
        return;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (nullptr ==  BB) 
        return;

    FVector CurrentLocation = Monster->GetActorLocation();
    FVector HomeLocation = BB->GetValueAsVector("HomeLocation");
    FSenseConfigData Config = Monster->GetSenseConfigData();
    float DistanceToHome = FVector::Dist(CurrentLocation, HomeLocation);

    bool bIsReturning = BB->GetValueAsBool("IsReturning");

    if (false == bIsReturning)
    {
        if (DistanceToHome > Config.PatrolRadius * 1.5f)
        {
            BB->SetValueAsBool("IsReturning", true);
        }
    }
    else
    {
        // 복귀 중인데 홈에 거의 도착했다면 복귀 모드 종료 -> 이제부터 다시 정찰 가능
        if (DistanceToHome <= 100.0f)
        {
            BB->SetValueAsBool("IsReturning", false);
        }
    }

    if (auto* Movement = Monster->GetCharacterMovement())
    {
        if(Movement->MaxWalkSpeed != Config.IdleSpeed)
          Movement->MaxWalkSpeed = Config.IdleSpeed;
    }
}
