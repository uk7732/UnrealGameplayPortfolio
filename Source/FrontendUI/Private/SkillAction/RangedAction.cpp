// Vince Petrelli All Rights Reserved


#include "SkillAction/RangedAction.h"
#include "Component/SkillComponent.h"
#include "Controllers/CC_PlayerController.h"
#include "Controllers/CC_AIController.h"
#include "Characters/CC_MonsterCharacter.h"
#include "Actor/RangedEffect/RangedEffectBase.h"

bool URangedAction::SkillAction(ACC_CharacterBase* owner)
{
    if (nullptr == owner || !owner->HasAuthority()) 
        return false;

	if (nullptr == owner)
		return false;

	FSkillTableRow SkillTable = owner->GetCurrentSkillData();

	if (SkillTable.MontageID.IsNone())
		return false;

	USkillComponent* skillComp = owner->GetSkillComp();
	if (nullptr == skillComp)
		return false;
	//쿨타임 리셋
	skillComp->CurrentSkillResetCoolDown();
	//마나소모
	owner->ApplyMP(-SkillTable.CostMana);

    //플레이어인 경우 총알 소모
    ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(owner);
    if (player)
    {
        player->UsedCurrentAmmo();
    }

	FVector TargetLocation = FVector::ZeroVector;
	FVector OwnerLocation = owner->GetActorLocation();
	float MaxRange = SkillTable.RangedEffectBehaviorData.Range;
	AController* OwnerController = owner->GetController();
	ACC_PlayerController* PlayerController = Cast<ACC_PlayerController>(OwnerController);

    if (PlayerController)
    {
        // 플레이어 (마우스 커서의 월드 평면 교차점 사용)
        FVector WorldLocation; // 카메라 위치
        FVector WorldDirection; // 마우스 커서 방향 벡터

        // 마우스 위치를 월드 좌표로 디프로젝트
        PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

        // 시전자 Z축 높이(OwnerLocation.Z)를 지나는 평면과 광선(Ray)의 교차점을 계산합니다.
        // 이 로직은 지형과 관계없이 마우스 커서 아래의 3D 공간 위치를 결정합니다.

        // 주의: WorldDirection.Z가 0에 가까우면 수평에 가까워져 에러가 발생할 수 있음
        if (FMath::IsNearlyZero(WorldDirection.Z))
        {
            // 수평에 가까울 경우, 시전자 정면 최대 사거리로 폴백 (Fall-back) 처리
            TargetLocation = OwnerLocation + owner->GetActorForwardVector() * MaxRange;
        }
        else
        {
            // (Z_owner - Z_cam) / D_ray.z : 광선이 평면(Z_owner)에 도달하는 데 필요한 거리(t) 계산
            float DistanceToPlane = (OwnerLocation.Z - WorldLocation.Z) / WorldDirection.Z;

            // WorldLocation에서 WorldDirection으로 t만큼 이동한 지점이 목표 위치입니다.
            TargetLocation = WorldLocation + WorldDirection * DistanceToPlane;
        }
    }
    else
    {
        ACC_AIController* controller = Cast<ACC_AIController>(OwnerController);
        if (nullptr == controller)
            return false;
        ACC_MonsterCharacter* monster = Cast<ACC_MonsterCharacter>(owner);
        if (nullptr == monster)
            return false;

        TargetLocation = monster->GetSaveSkillLocation();

        FVector DirectionToTarget = TargetLocation - OwnerLocation;

    }


    // ----------------------------------------------------
    // 3.최대 사거리 제한 (Clamping) - 이 로직이 핵심
    // ----------------------------------------------------
    FVector Direction = TargetLocation - OwnerLocation;
    float Distance = Direction.Size();

    if (Distance > MaxRange)
    {
        // 커서가 사거리를 초과하는 지점을 가리키더라도, 
        // 해당 방향의 끝 사거리(MaxRange) 지점으로 위치를 제한합니다.
        TargetLocation = OwnerLocation + Direction.GetSafeNormal() * MaxRange;
    }

    // ----------------------------------------------------
    // 4. 이펙트 액터 스폰 및 초기화
    // ----------------------------------------------------
    if (owner->HasAuthority() && SkillTable.RangedEffectBehaviorData.RangedEffect)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = owner;
        SpawnParams.Instigator = owner;
        FName SkillTID = owner->GetCurrentSkillTID();
        // ... (Spawn Params 설정) ...

        ARangedEffectBase* SpawnedEffect = GetWorld()->SpawnActor<ARangedEffectBase>(
            SkillTable.RangedEffectBehaviorData.RangedEffect,
            TargetLocation,
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (SpawnedEffect)
        {
            SpawnedEffect->InitializeFromData(SkillTID, OwnerController);
            return true;
        }
    }

	return false;
}
