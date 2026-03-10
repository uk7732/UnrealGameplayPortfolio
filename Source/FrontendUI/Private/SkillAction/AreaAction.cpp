// Vince Petrelli All Rights Reserved


#include "SkillAction/AreaAction.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h" // 가끔 필요한 유틸리티 헤더
#include "Kismet/GameplayStatics.h"
#include "Component/Skillcomponent.h"
#include "Engine/OverlapResult.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controllers/CC_AIController.h"

bool UAreaAction::SkillAction(ACC_CharacterBase* owner)
{
	if (nullptr == owner || !owner->HasAuthority())
		return false;

	if (nullptr == owner)
		return false;

	FSkillTableRow SkillTable = owner->GetCurrentSkillData();

	FAreaActionData AreaData = SkillTable.AreaActionData;

	if (SkillTable.MontageID.IsNone())
		return false;

	USkillComponent* skillComp = owner->GetSkillComp();
	if (nullptr == skillComp)
		return false;
	//쿨타임 리셋
	skillComp->CurrentSkillResetCoolDown();
	//마나소모
	owner->ApplyMP(-SkillTable.CostMana);



	TArray<ACC_CharacterBase*> FinalTargets;
	FVector CheckLocation = owner->GetActorLocation();
	
	ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(owner);
	if (Monster)
	{
		ACC_AIController* Controller = Cast<ACC_AIController>(Monster->GetController());
		if (Controller && Controller->GetBlackboardComponent())
		{
			AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
			if (Target)
			{
				// 판정의 기준 높이를 타겟의 허리 높이(Z + 50)로 설정
				CheckLocation.Z = Target->GetActorLocation().Z + 50.f;
			}
		}
	}

	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner); // 일단 owner는 제외하고 찾음 (나중에 필요하면 직접 데미지)


	//콘형태 타격
	if (AreaData.ConeHalfAngle > 0.0f)
	{
		FVector ownerForward = owner->GetActorForwardVector().GetSafeNormal();
		float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(AreaData.ConeHalfAngle));
		float HalfAngleRad = FMath::DegreesToRadians(AreaData.ConeHalfAngle);


		bool overlapResult = GetWorld()->OverlapMultiByChannel(
			overlaps,
			CheckLocation,
			FQuat::Identity,
			ECC_GameTraceChannel1, // Sweep과 동일한 채널 사용
			FCollisionShape::MakeSphere(AreaData.MaxDistance),
			params
		);

		FVector StartPointA = CheckLocation + ownerForward * AreaData.MinDistance;

		if (overlapResult)
		{
			for (const FOverlapResult& overlap : overlaps)
			{
				ACC_CharacterBase* defender = Cast<ACC_CharacterBase>(overlap.GetActor());
				if (false == IsValid(defender))
					continue;

				// 팀 체크 (태그 활용) - 아군은 제외
				bool bIsSameTeam = (owner->ActorHasTag(TEXT("Player")) && defender->ActorHasTag(TEXT("Player"))) ||
					               (owner->ActorHasTag(TEXT("Monster")) && defender->ActorHasTag(TEXT("Monster")));
				
				if (bIsSameTeam) 
					continue;

				// 1. TargetVector 및 Direction 계산 (ownerLocation 기준)
				FVector TargetVector = (defender->GetActorLocation() - CheckLocation);
				TargetVector.Z = 0.0f;

				FVector TargetDirection = TargetVector.GetSafeNormal();
				float DotProduct = FVector::DotProduct(ownerForward, TargetDirection);

				// 2. 각도 검사 (타겟이 콘 각도 내에 있는가?)
				if (DotProduct >= CosHalfAngle)
				{
					// ---  MinDistance 검증 (StartPointA 기준)  ---

					// StartPointA에서 타겟까지의 벡터
					FVector Vector_A_to_Target = defender->GetActorLocation() - StartPointA;

					// StartPointA를 기준으로 타겟이 OwnerForward 방향으로 얼마나 앞에 있는지 투영 길이 계산
					float ProjectionLength = FVector::DotProduct(Vector_A_to_Target, ownerForward);

					// ProjectionLength가 0.0f 이상이면, 타겟이 StartPointA 지점(MinDistance)보다 앞쪽에 있음
					if (ProjectionLength >= 0.0f)
					{
						FinalTargets.AddUnique(defender);
					}
				}
			}
		}
		// 디버그 드로잉 (원뿔 모양)
#ifdef ENABLE_DRAW_DEBUG
	//// 콘의 시작점은 StartPointA
	//	FVector ConeStart = StartPointA;

	//	// 콘의 길이 (MaxDistance에서 MinDistance를 뺀 실제 유효 길이)
	//	float ConeLength = AreaData.MaxDistance - AreaData.MinDistance;

	//	// FColor::Red, 1.0f (LifeTime), 10.0f (Thickness)
	//	DrawDebugCone(
	//		GetWorld(),
	//		ConeStart,
	//		ownerForward,
	//		ConeLength, // 콘의 길이
	//		HalfAngleRad,
	//		HalfAngleRad,
	//		32,
	//		FColor::Red,
	//		false,
	//		3.0f, // 3초 동안 표시
	//		(uint8)ECC_WorldStatic,
	//		10.0f
	//	);
#endif
	}
	//선형타격
	else
	{
		FVector Forward = owner->GetActorForwardVector();
		Forward.Z = CheckLocation.Z;
		Forward.Normalize();
		float MinDist = AreaData.MinDistance;
		float MaxDist = AreaData.MaxDistance;
		float TotalLength = MaxDist - MinDist;
		float HalfLength = TotalLength * 0.5f;
		float HalfWidth = AreaData.Range * 0.5f;


		FVector CenterLocation = CheckLocation + Forward * (MinDist + HalfLength);
		// BoxExtent는 '반지름' 개념입니다.
		FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(HalfLength, HalfWidth, 100.f));

		TArray<FOverlapResult> result;
		FQuat Rotation = Forward.Rotation().Quaternion();

		bool collision = GetWorld()->OverlapMultiByChannel(
			result,
			CenterLocation,
			Rotation,
			ECC_GameTraceChannel1,
			BoxShape,
			params
		);
		//디버그일때만 그림
#ifdef ENABLE_DRAW_DEBUG
//// 시작점: 실제 박스 판정이 시작되는 MinDistance 지점
//		FVector LineStart = CheckLocation + (Forward * AreaData.MinDistance);
//		// 끝점: 실제 박스 판정이 끝나는 MaxDistance 지점
//		FVector LineEnd = CheckLocation + (Forward * AreaData.MaxDistance);
//
//		FColor drawColor = collision ? FColor::Red : FColor::Green;
//
//		// 두께(Thickness)를 AreaData.Range(너비)만큼 주면 박스처럼 보입니다.
//		DrawDebugLine(
//			GetWorld(),
//			LineStart,
//			LineEnd,
//			drawColor,
//			false,
//			1.f,
//			0,
//			AreaData.Range // 선의 두께를 공격 너비만큼 설정
//		);
#endif
		if (collision)
		{
			for (const FOverlapResult& hit : result)
			{
				ACC_CharacterBase* defender = Cast<ACC_CharacterBase>(hit.GetActor());
				if (IsValid(defender))
				{
					bool bIsSameTeam = (owner->ActorHasTag(TEXT("Player")) && defender->ActorHasTag(TEXT("Player"))) ||
						               (owner->ActorHasTag(TEXT("Monster")) && defender->ActorHasTag(TEXT("Monster")));

					if(bIsSameTeam == false)
						FinalTargets.AddUnique(defender);
				}
			}
		}
	}

	if (FinalTargets.Num() > 0)
	{
		for (ACC_CharacterBase* defender : FinalTargets)
		{
			if (false == IsValid(defender)) continue;

			const float DefenceConstantK = 100.0f;
			float Skilldamage = SkillTable.Damage;
			float OwnerAttackStats = 0.0f;

			const FAbilityData* AttackData = owner->GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_ATTACK);
			if (AttackData != nullptr) OwnerAttackStats = AttackData->Value;

			// Projectile과 동일한 공식 적용: 기본 데미지로 시작
			float FinalSkillDamage = Skilldamage;
			float ArmPen = 0.0f;

			for (auto& it : SkillTable.Rates)
			{
				if (it.Type == eAbilType::ABIL_TYPE_ATTACK)
				{
					// 합산 방식 통일: 기본 데미지 + (내 공격력 * 계수)
					FinalSkillDamage += (OwnerAttackStats * it.Rate);
				}
				else if (it.Type == eAbilType::ABIL_TYPE_DEFENCE)
				{
					// TakeDamage쪽의 곱연산(0.01)과 동일하게 맞춤
					ArmPen = it.Rate * 0.01f;
					ArmPen = FMath::Clamp(ArmPen, 0.0f, 0.9f);
				}
			}

			float DefenderDefanceStats = 0.0f;
			const FAbilityData* DefenceData = defender->GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_DEFENCE);
			if (DefenceData != nullptr) DefenderDefanceStats = DefenceData->Value;

			float EffectiveDefence = DefenderDefanceStats * (1.0f - ArmPen);
			EffectiveDefence = FMath::Max(0.0f, EffectiveDefence);

			float DamageReductionRatio = EffectiveDefence / (EffectiveDefence + DefenceConstantK);

			// 방어력이 깎인 최종 결과 데미지 계산
			float ResultDamage = FinalSkillDamage * (1.0f - DamageReductionRatio);
			ResultDamage = FMath::Max(1.0f, ResultDamage);

			UGameplayStatics::ApplyDamage(
				defender,
				ResultDamage,
				owner->GetController(),
				owner,
				UDamageType::StaticClass()
			);
		}
		return true;
	}
	return false;
}
