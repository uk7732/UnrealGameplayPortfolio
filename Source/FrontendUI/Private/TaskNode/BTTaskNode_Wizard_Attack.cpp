// Vince Petrelli All Rights Reserved

#include "TaskNode/BTTaskNode_Wizard_Attack.h"
#include "Characters/CC_MonsterCharacter.h"
#include "Controllers/CC_AIController.h"
#include "Component/SkillComponent.h"
#include "Instance/Admin4_GameInstance.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTaskNode_Wizard_Attack::UBTTaskNode_Wizard_Attack()
{
    NodeName = TEXT("Attack");
    bNotifyTick = false;
}

EBTNodeResult::Type UBTTaskNode_Wizard_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    ACC_AIController* AIController = Cast<ACC_AIController>(OwnerComp.GetAIOwner());
    if (!AIController)
        return EBTNodeResult::Failed;

    ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(AIController->GetPawn());
    if (!Monster)
        return EBTNodeResult::Failed;

    USkillComponent* SkillComp = Monster->GetSkillComp();
    if (nullptr == SkillComp)
        return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (nullptr == BB)
        return EBTNodeResult::Failed;

    BB->SetValueAsBool("IsAttacking", true);


    FName SkillTID = BB->GetValueAsName("SkillTID");
    if (SkillTID.IsNone())
    {
        return EBTNodeResult::Failed;
    }

    Monster->OnAttackAnimationEnded.Clear();
    TWeakObjectPtr<UBehaviorTreeComponent> WeakOwnerComp(&OwnerComp);

    Monster->OnAttackAnimationEnded.AddWeakLambda(
        this,
        [this, WeakOwnerComp](bool bInterrupted)
        {
            if (!WeakOwnerComp.IsValid()) return;

            UBlackboardComponent* BlockB = WeakOwnerComp->GetBlackboardComponent();

            if (BlockB)
            {
                BlockB->SetValueAsBool("IsAttacking", false);
                BlockB->SetValueAsName("SkillTID", NAME_None);
                BlockB->SetValueAsBool("SkillInit", false);
            }

            FinishLatentTask(
                *WeakOwnerComp,
                bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded
            );
        }
    );


    Monster->SkillAction(SkillTID);

    return EBTNodeResult::InProgress;
}

void UBTTaskNode_Wizard_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTaskNode_Wizard_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)

{
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

    if (ACC_AIController* AI = Cast<ACC_AIController>(OwnerComp.GetAIOwner()))
    {
        if (ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(AI->GetPawn()))
        {
            Monster->OnAttackAnimationEnded.Clear();
        }
    }
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (nullptr == BB)
        return;

    BB->SetValueAsBool("SkillInit", false);
}
