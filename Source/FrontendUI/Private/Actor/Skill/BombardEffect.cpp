// Vince Petrelli All Rights Reserved


#include "Actor/Skill/BombardEffect.h"
#include "Actor/Projectile/ProjectileBase.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Kismet/KismetMathLibrary.h"

void ABombardEffect::InitializeFromData(const FName RangedEffectTID, AController* ownercontroller)
{
	Super::InitializeFromData(RangedEffectTID, ownercontroller); // 기본 세팅(반경, 시간 등)

	CachedSkillTID = RangedEffectTID;

	// 테이블 로드 시스템을 통해 ProjectileClass 정보를 가져옴
	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (TableSub)
	{
		FSkillTableRow* SkillTable = TableSub->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, RangedEffectTID);
		if (SkillTable)
		{
			// 이 장판이 사용할 투사체 클래스 저장
			RainProjectileClass = SkillTable->ProjectileBehaviorData.ProjectileClass;

			// 테이블의 '틱 데미지' 값을 -> '발사 간격'으로 재해석해서 사용
			DamageTickInterval = SkillTable->RangedEffectBehaviorData.DurationTickDamage;
		}
	}
}

void ABombardEffect::ApplyPeriodicDamage()
{
	// 서버만 실행, 투사체 없으면 취소
	if (!HasAuthority() || !RainProjectileClass) return;

	// 1. 장판 범위 내 랜덤 위치 구하기
	float Radius = RangedEffectArea->GetScaledSphereRadius();
	FVector2D RandomPoint = FMath::RandPointInCircle(Radius);
	FVector SpawnPos = GetActorLocation() + FVector(RandomPoint.X, RandomPoint.Y, 1000.0f); // 높이 1000

	// 2. 바닥을 향해 발사
	FRotator SpawnRot = FRotator(-90.0f, 0.0f, 0.0f);

	// 3. 투사체 스폰
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = GetInstigator();

	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(RainProjectileClass, SpawnPos, SpawnRot, Params);
	if (Projectile)
	{
		// 투사체 초기화 (데미지 등은 여기서 처리됨)
		Projectile->server_InitializeFromData(CachedSkillTID, OwnerController);
	}
}
