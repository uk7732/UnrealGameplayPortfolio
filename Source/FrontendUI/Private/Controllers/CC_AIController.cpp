// Vince Petrelli All Rights Reserved


#include "Controllers/CC_AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/CC_MonsterCharacter.h"
// AI Perception 컴포넌트
#include "Perception/AIPerceptionComponent.h"
// 시각 감각 설정 클래스 (UAISenseConfig_Sight)
#include "Perception/AISenseConfig_Sight.h"
// 자극 데이터 구조체 (FAIStimulus)
#include "Perception/AIPerceptionTypes.h"
#include "Characters/CC_PlayerCharacter.h"

ACC_AIController::ACC_AIController()
{
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    // 설정 카드(SightConfig) 생성 - UAISenseConfig_Sight 자료형 사용
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    // 무엇을 적으로 간주할지 설정 (모두 감지)
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // 수신기에 설정 카드 삽입
    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

}

void ACC_AIController::RunAI(UBehaviorTree* InBTAsset)
{
    if (BTAsset)
    {
        RunBehaviorTree(InBTAsset);
    }
}

void ACC_AIController::BeginPlay()
{
    Super::BeginPlay();
}

void ACC_AIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
  
}

AActor* ACC_AIController::GetNearestDetectedActor()
{

    ACC_PlayerCharacter* BestTarget = nullptr;

    if (PlayerActors.Num() > 0)
    {
        float ClosestDistSq = MAX_FLT;
        FVector Origin = GetPawn()->GetActorLocation();

        for (ACC_PlayerCharacter* Player : PlayerActors)
        {
            if (IsValid(Player))
            {
                float DistSq = FVector::DistSquared(Origin, Player->GetActorLocation());
                if (DistSq < ClosestDistSq)
                {
                    ClosestDistSq = DistSq;
                    BestTarget = Player;
                }
            }
        }
    }

    if (BestTarget)
    {
        Blackboard->SetValueAsObject(TEXT("TargetActor"), BestTarget);
        Blackboard->SetValueAsVector(TEXT("TargetActorLocation"), BestTarget->GetActorLocation());
    }
    else
    {
        Blackboard->ClearValue(TEXT("TargetActor"));
    }

    return BestTarget;
}

void ACC_AIController::OnTargetDetected(AActor* actor, FAIStimulus Stimulus)
{
    if (Blackboard == nullptr)
        return;

    if (actor == nullptr)
        return;

    if (!actor->ActorHasTag(TEXT("Player")))
        return;

    ACC_PlayerCharacter* playerlist = Cast<ACC_PlayerCharacter>(actor);
    if (nullptr == playerlist)
        return;

    if (Stimulus.WasSuccessfullySensed())
    {
        // 시야에 들어옴: 배열에 추가
        PlayerActors.AddUnique(playerlist);
    }
    else
    {
        // 시야에서 사라짐: 배열에서 제거
        PlayerActors.Remove(playerlist);
    }


    GetNearestDetectedActor();
}

void ACC_AIController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ACC_AIController::OnTargetDetected);
    }
}

void ACC_AIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);


    ACC_MonsterCharacter* monster = Cast<ACC_MonsterCharacter>(InPawn);
    if (nullptr == monster)
        return;

    BTAsset = monster->GetBehaviorTree();
    if (nullptr == BTAsset)
        return;
    BBAsset = BTAsset->GetBlackboardAsset();
    if (nullptr == BBAsset)
        return;

    UBlackboardComponent* NewBBComp = nullptr;

    FSenseConfigData SenseConfigData = monster->GetSenseConfigData();

    SightConfig->SightRadius = SenseConfigData.SightRadius;                                   // 인식 거리
    SightConfig->LoseSightRadius = SenseConfigData.LoseSightRadius;                           // 놓치는 거리
    SightConfig->PeripheralVisionAngleDegrees = SenseConfigData.PeripheralVisionAngleDegrees; // 시야각 (중심에서 60도, 총 120도)

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    if (UseBlackboard(BBAsset, NewBBComp))
    {

        Blackboard = NewBBComp;
        // 홈 위치 기억하기
        Blackboard->SetValueAsVector(TEXT("HomeLocation"), InPawn->GetActorLocation());

        if (!RunBehaviorTree(BTAsset))
        {
            UE_LOG(LogTemp, Error, TEXT("AIController couldn't run behavior tree!"));
        }
    }
}
