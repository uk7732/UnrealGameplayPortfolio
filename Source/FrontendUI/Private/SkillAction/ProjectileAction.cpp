// Vince Petrelli All Rights Reserved


#include "SkillAction/ProjectileAction.h"
#include "Actor/Projectile/ProjectileBase.h"
#include "Common/GameDatas.h"
#include "Component/SkillComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controllers/CC_AIController.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"

#include "FrontendDebugHelper.h"

bool UProjectileAction::SkillAction(ACC_CharacterBase* owner)
{
    if (nullptr == owner || !owner->HasAuthority())
        return false;

    // 1. 기본 유효성 검사
    if (nullptr == owner) return false;

    FSkillTableRow SkillTable = owner->GetCurrentSkillData();
    // 몽타주 ID가 없으면 실행 불가
    // if (SkillTable.MontageID.IsNone()) return false; // (필요시 주석 해제, 데이터에 따라 다름)

    USkillComponent* skillComp = owner->GetSkillComp();
    if (nullptr == skillComp) return false;

    // 2. 쿨타임 및 마나 처리
    skillComp->CurrentSkillResetCoolDown();
    owner->ApplyMP(-SkillTable.CostMana);

    // 3. 플레이어라면 탄약 소모
    ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(owner);

    // 발사 위치/회전 계산 변수 미리 선언
    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    FVector Direction = owner->GetActorForwardVector();

    FProjectileBehaviorData ProjectileData = SkillTable.ProjectileBehaviorData;

    if (player)
    {
        // (1) 탄약 체크
        if (player->GetCurrentAmmoCount() <= 0)
        {
            Debug::Print(TEXT("탄약 부족! 발사 취소"), -1, FColor::Red);
            return false;
        }

        // (2) 탄약 소모
        player->UsedCurrentAmmo();

        // (3) 로그 출력
        int32 InventoryAmmo = 0;
        ACC_PlayerState* PS = player->GetPlayerState<ACC_PlayerState>();
        if (PS && PS->GetInventory())
        {
            InventoryAmmo = PS->GetInventory()->GetAmmoData().Count;
        }
        FString Msg = FString::Printf(TEXT("발사성공! 장전: %d, 인벤: %d"),
            player->GetCurrentAmmoCount(), InventoryAmmo);
        Debug::Print(Msg, -1, FColor::Green);

        // (4) 플레이어 발사 위치 계산 (탑다운 시점 고려)
        FVector OwnerLocation = owner->GetActorLocation();
        FVector OwnerForwardVector = owner->GetActorForwardVector();
        FVector OwnerRightVector = owner->GetActorRightVector();

        FVector Height = FVector(0.f, 0.f, 50.f); // 높이 약간 조정 (30 -> 80, 바닥 충돌 방지)

        SpawnLocation = OwnerLocation +
            (OwnerForwardVector * ProjectileData.Forward) +
            (OwnerRightVector * ProjectileData.RIght) +
            Height;
    }
    else
    {
        // 몬스터 AI 발사 로직 (기존 코드 유지)
        ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(owner);
        if (Monster)
        {
            ACC_AIController* AIController = Cast<ACC_AIController>(Monster->GetController());
            if (AIController && AIController->GetBlackboardComponent())
            {
                AActor* Target = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
                if (Target)
                {
                    FVector TargetLoc = Target->GetActorLocation();
                    FVector MonsterLoc = Monster->GetActorLocation();

                    Direction = TargetLoc - MonsterLoc;
                    Direction.Z = 0.f;
                    Direction.Normalize();

                    float ShootHeight = TargetLoc.Z + 50.f; // 타겟 높이 고려
                    SpawnLocation = MonsterLoc + (Direction * ProjectileData.Forward);
                    SpawnLocation.Z = ShootHeight;
                }
            }
        }
    }

    // 4. 투사체 생성 (SpawnActor)
    // 이 부분이 없어서 투사체가 안 나갔던 것입니다.
    SpawnRotation = Direction.Rotation();
    FName SkillTID = owner->GetCurrentSkillTID();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = owner;
    SpawnParams.Instigator = owner->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 실제 월드에 투사체 생성
    AProjectileBase* Projectile = owner->GetWorld()->SpawnActor<AProjectileBase>(
        ProjectileData.ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    // 5. 생성 결과 처리
    if (Projectile)
    {
        // 발사자와의 충돌 무시 설정
        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Projectile->GetRootComponent()))
        {
            RootComp->IgnoreActorWhenMoving(owner, true);
        }
        owner->MoveIgnoreActorAdd(Projectile);

        // 데이터 초기화
        Projectile->InitializeFromData(SkillTID, owner->GetController());

        return true;
    }
    else
    {
        Debug::Print(TEXT("투사체 생성 실패! (클래스 NULL 혹은 위치 문제)"), -1, FColor::Red);
    }

    return false;
}
