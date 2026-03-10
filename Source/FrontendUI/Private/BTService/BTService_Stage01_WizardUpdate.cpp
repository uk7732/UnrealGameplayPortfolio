// Vince Petrelli All Rights Reserved


#include "BTService/BTService_Stage01_WizardUpdate.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Controllers/CC_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h" 
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Characters/CC_MonsterCharacter.h"
#include "Component/SkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"


UBTService_Stage01_WizardUpdate::UBTService_Stage01_WizardUpdate()
{
	NodeName = TEXT("Update Distance");

	bNotifyTick = true;
}

void UBTService_Stage01_WizardUpdate::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Monster)
		return;

	FSenseConfigData Data = Monster->GetSenseConfigData();

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (nullptr == BB)
		return;

	ACC_AIController* AIC = Cast<ACC_AIController>(OwnerComp.GetAIOwner());
	if (nullptr == AIC)
		return;

	AActor* ClosestPlayer = AIC->GetNearestDetectedActor();

	if (ClosestPlayer)
	{
		// 기존 타겟과 다르다면 블랙보드 교체 (실시간 어그로 전환)
		AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
		if (ClosestPlayer != CurrentTarget)
		{
			BB->SetValueAsObject("TargetActor", ClosestPlayer);
		}
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!IsValid(Target))
	{
		BB->SetValueAsBool("InAttackRange", false);
		BB->SetValueAsBool("InAggroRange", false);
		BB->SetValueAsBool("SkillInit", false);
		BB->ClearValue("TargetActor");
		BB->SetValueAsFloat("FollowTime", 0.0f);
		return;
	}

	float CurrentFollowTime = BB->GetValueAsFloat("FollowTime");
	CurrentFollowTime += DeltaSeconds;
	BB->SetValueAsFloat("FollowTime", CurrentFollowTime);

	if (CurrentFollowTime >= 10.f || !BB->GetValueAsBool("SkillInit"))
	{
		USkillComponent* SkillComp = Monster->GetSkillComp();
		BB->SetValueAsFloat("AttackRange", Data.AttackRange);
		if (SkillComp)
		{
			TArray<FSkillData> SkillDatas = SkillComp->GetSkillDatas();

			FSkillData BestSkillData = {};
			bool InitSkillSet = false;

			for (const auto& it : SkillDatas)
			{
				if (it.CurrentCoolDown != 0)
					continue;

				if (false == InitSkillSet)
				{
					InitSkillSet = true;
					BestSkillData = it;
				}
				else
				{
					int32 RandNum = FMath::RandRange(0, 1);
					if (RandNum == 0)
					{
						BestSkillData = it;
					}
				}
			}

			if (!BestSkillData.TID.IsNone())
			{
				BB->SetValueAsName("SkillTID", BestSkillData.TID);
				BB->SetValueAsBool("SkillInit", true);

			}
			else
			{
				BB->SetValueAsName("SkillTID", NAME_None);
				BB->SetValueAsBool("SkillInit", false);
			}

			BB->SetValueAsFloat("FollowTime", 0.0f);
		}
	}

	UCharacterMovementComponent* Movement = Monster->GetCharacterMovement();
	if (Movement && Movement->MaxWalkSpeed != Data.CombatSpeed)
	{
		Movement->MaxWalkSpeed = Data.CombatSpeed;
	}


	float CurrentTargetRange = BB->GetValueAsFloat("AttackRange");

	if (CurrentTargetRange <= 0.f)
		CurrentTargetRange = Data.AttackRange;

	float Distance = FVector::Dist2D(Monster->GetActorLocation(), Target->GetActorLocation());

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bIsTargetOnNavMesh = false;
	if (NavSys)
	{
		FNavLocation ProjectedLocation;
		// 타겟 위치 주변에 유효한 NavMesh가 있는지 투영(Project)해서 확인
		// Extent(반경)는 상황에 맞게 조절하세요. (여기서는 가로 100, 세로 500 정도)
		bIsTargetOnNavMesh = NavSys->ProjectPointToNavigation(Target->GetActorLocation(), ProjectedLocation, FVector(100.f, 100.f, 500.f));
	}


	bool bIsAttacking = BB->GetValueAsBool("IsAttacking");

	float FinalRange = CurrentTargetRange;

	if (bIsAttacking)
	{
		// 이미 때리는 중이면 300까지는 "범위 안"으로 인정 (추격 재개 방지)
		FinalRange += 500.0f;
	}
	else
	{
		// 공격 시작 전에는 확실히 다가가야 함 (움찔거림 방지를 위해 50 정도만 여유)
		FinalRange += 100.0f;
	}

	bool bCanAttack = (Distance <= FinalRange) && bIsTargetOnNavMesh;

	BB->SetValueAsBool("InAttackRange", bCanAttack);
	BB->SetValueAsBool("InAggroRange", Distance <= Data.SightRadius);
}
