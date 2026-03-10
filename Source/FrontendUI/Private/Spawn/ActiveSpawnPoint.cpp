// Vince Petrelli All Rights Reserved


#include "Spawn/ActiveSpawnpoint.h"
#include "Instance/TableSubsystem.h"
#include "Characters/CC_MonsterCharacter.h"
#include "Component/SpawnManagerComponent.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CC_NPCCharacter.h"
UActiveSpawnPoint::UActiveSpawnPoint()
{


}

void UActiveSpawnPoint::RequestSpawnAction()
{
	if (SpawnZoneData.MaxSpawnCount == SpawnZoneData.CurrentSpawnCount)
		return;

	server_RequestSpawnAction();
}

bool UActiveSpawnPoint::InitializerActiveSpawnPoint(const FName& SpawnTID)
{
	UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
	if (nullptr == Tablesystem)
		return false;

	FSpawnTableRow* SpawnTable = Tablesystem->FindTableRow<FSpawnTableRow>(TABLE_NAME::SPAWN, SpawnTID);
	if (nullptr == SpawnTable)
		return false;

	Type = SpawnTable->Type;
	CharacterType = SpawnTable->CharacterType;
	SpawnZoneData.SpawnName = SpawnTable->SpawnName;
	SpawnZoneData.CharacterTID = SpawnTable->MonsterTID;
	SpawnZoneData.Location = SpawnTable->SpawnLocation;
	SpawnZoneData.Rotation = SpawnTable->SpawnRotation;
	SpawnZoneData.SpawnRadiusOrSide = SpawnTable->SpawnRadiusOrSide;
	SpawnZoneData.MaxSpawnCount = SpawnTable->SpawnCount;
	SpawnZoneData.SpawnInterval = SpawnTable->SpawnInterval;
	SpawnZoneData.RoomNumber = SpawnTable->SpawnRoomNumber;
	bIsTriggerSpawnType = SpawnTable->bIsTriggerSpawn;
	return true;
}

bool UActiveSpawnPoint::IsIntervalTimeCheck(float IntervalTime)
{
	float& CurrentIntervalTime = SpawnZoneData.CurrentIntervalTime;
	float SpawnIntervalTime = SpawnZoneData.SpawnInterval;

	CurrentIntervalTime += IntervalTime;

	if (CurrentIntervalTime >= SpawnIntervalTime)
	{
		CurrentIntervalTime = 0.0f;
		return true;
	}

	return false;
}

const FName& UActiveSpawnPoint::GetSpawnName()
{
	return SpawnZoneData.SpawnName;
}

float UActiveSpawnPoint::GetSpawnInterval()
{
	return SpawnZoneData.SpawnInterval;
}

bool UActiveSpawnPoint::GetIsTriggerSpawnType()
{
	return bIsTriggerSpawnType;
}

void UActiveSpawnPoint::SetupCharacter(ACC_CharacterBase* NewCharacter)
{
	if (nullptr == NewCharacter)
		return;
	
	SpawnCharacters.Add(NewCharacter);

	++SpawnZoneData.CurrentSpawnCount;

	if (eSpawnCharacterType::SPAWN_MONSTER == CharacterType)
	{
		ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(NewCharacter);
		if (nullptr == Monster)
			return;
		Monster->OnMonsterDeath.AddDynamic(this, &UActiveSpawnPoint::HandleMonsterDeath);
	}
		
}

void UActiveSpawnPoint::HandleMonsterDeath(ACC_MonsterCharacter* DeadMonster)
{
	USpawnManagerComponent* Comp = Cast<USpawnManagerComponent>(GetOuter());
	if (nullptr == Comp)
		return;

	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		SpawnCharacters.RemoveSingle(DeadMonster);

		--SpawnZoneData.CurrentSpawnCount;
	}
}

void UActiveSpawnPoint::SinglePoint_SpawnAction()
{
	FVector ProposedLocation = SpawnZoneData.Location;
	FRotator Rotation = SpawnZoneData.Rotation;

	// 최종 스폰 위치를 저장할 변수 선언
	FVector FinalSpawnLocation;

	// 유효성 검사 및 최종 위치 획득 시도
	if (!TryGetValidSpawnLocation(ProposedLocation, FinalSpawnLocation))
		return;

	switch (CharacterType)
	{
	case eSpawnCharacterType::SPAWN_MONSTER :
		SpawnAndSetupMonster(FinalSpawnLocation, Rotation);
		break;
	case eSpawnCharacterType::SPAWN_NPC : 
		SpawnAndSetupNPC(FinalSpawnLocation, Rotation);
		break;
	default:
		break;
	}
}

void UActiveSpawnPoint::RandomCircle_SpawnAction()
{
	int32 TryCount = 0;

	const int32 NumToSpawn = SpawnZoneData.MaxSpawnCount;
	while (NumToSpawn != SpawnZoneData.CurrentSpawnCount)
	{
		if (TryCount == 10)
		{
			FString MonsterID = FString::Printf(TEXT("%s"), *SpawnZoneData.CharacterTID.ToString());
			UE_LOG(LogTemp, Warning, TEXT("%s Circle 스폰 실패"), *MonsterID);
			return;
		}

		const FRotator SpawnRotator = SpawnZoneData.Rotation;
		const FVector CenterLocation = SpawnZoneData.Location;
		const float Radius = SpawnZoneData.SpawnRadiusOrSide;

		// 랜덤 각도 및 거리 계산
		// 0도에서 360도 사이의 랜덤 각도를 라디안으로 변환
		const float RandomAngleRad = FMath::RandRange(0.0f, 360.0f) * (PI / 180.0f);

		// 원의 중심부터 Radius 내의 랜덤 거리 (균일한 분포를 위해 거리의 제곱근 사용)
		const float RandomDistance = FMath::Sqrt(FMath::FRand()) * Radius;

		// 2D 좌표 (X, Y) 계산
		// FMath::Cos, FMath::Sin을 사용하여 극좌표를 직교좌표로 변환
		const float DeltaX = RandomDistance * FMath::Cos(RandomAngleRad);
		const float DeltaY = RandomDistance * FMath::Sin(RandomAngleRad);

		// 최종 스폰 위치 (Z축은 유지 또는 지면 높이로 설정)
		FVector SpawnLocation = CenterLocation;
		SpawnLocation.X += DeltaX;
		SpawnLocation.Y += DeltaY;

		FVector FinalSpawnLocation;

		if (!TryGetValidSpawnLocation(SpawnLocation, FinalSpawnLocation))
		{
			TryCount++;
			continue;
		}
		TryCount = 0;

		SpawnAndSetupMonster(FinalSpawnLocation, SpawnRotator);
	}
}

void UActiveSpawnPoint::RandomRectangle_SpawnAction()
{
	int32 TryCount = 0;

	const int32 NumToSpawn = SpawnZoneData.MaxSpawnCount;
	while (NumToSpawn != SpawnZoneData.CurrentSpawnCount)
	{
		if (TryCount == 10)
		{
			FString MonsterID = FString::Printf(TEXT("%s"), *SpawnZoneData.CharacterTID.ToString());
			UE_LOG(LogTemp, Warning, TEXT("%s Rectangle스폰 실패"), *MonsterID);
			return;
		}

		FRotator SpawnRotator = SpawnZoneData.Rotation;
		const FVector CenterLocation = SpawnZoneData.Location;
		// SpawnRadiusOrSide를 사각형의 '절반 길이(Half Side)'로 사용한다고 가정
		const float HalfSide = SpawnZoneData.SpawnRadiusOrSide / 2.0f;

		// 2. X축과 Y축의 랜덤 오프셋 계산
		// 중심(0)을 기준으로 -HalfSide 부터 +HalfSide 사이의 랜덤 값 생성
		const float RandomOffsetX = FMath::FRandRange(-HalfSide, HalfSide);
		const float RandomOffsetY = FMath::FRandRange(-HalfSide, HalfSide);

		// 3. 최종 스폰 위치 (Z축은 유지)
		FVector SpawnLocation = CenterLocation;
		SpawnLocation.X += RandomOffsetX;
		SpawnLocation.Y += RandomOffsetY;
		FVector FinalSpawnLocation;

		if (!TryGetValidSpawnLocation(SpawnLocation, FinalSpawnLocation))
		{
			TryCount++;
			continue;
		}
		TryCount = 0;

		SpawnAndSetupMonster(FinalSpawnLocation, SpawnRotator);
	}
}

void UActiveSpawnPoint::SpawnAndSetupMonster(const FVector& Location, const FRotator& Rotation)
{
	USpawnManagerComponent* Comp = Cast<USpawnManagerComponent>(GetOuter());
	if (nullptr == Comp)
		return;

	UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
	if (nullptr == Tablesystem)
		return;

	FMonsterTableRow* MonsterTable = Tablesystem->FindTableRow<FMonsterTableRow>(TABLE_NAME::MONSTER, SpawnZoneData.CharacterTID);
	if (nullptr == MonsterTable)
		return;

	TSubclassOf<ACC_MonsterCharacter> monster = MonsterTable->MonsterClass;


	ACC_MonsterCharacter* Monster = Comp->GetWorld()->SpawnActor<ACC_MonsterCharacter>(monster, Location, Rotation);

	if (Monster)
	{
		Monster->SetMonsterData(SpawnZoneData.CharacterTID);
		Monster->SetSpawnRoomNum(SpawnZoneData.RoomNumber);
	}

	SetupCharacter(Monster);
}

void UActiveSpawnPoint::SpawnAndSetupNPC(const FVector& Location, const FRotator& Rotation)
{
	USpawnManagerComponent* Comp = Cast<USpawnManagerComponent>(GetOuter());
	if (nullptr == Comp)
		return;

	UWorld* World = Comp->GetWorld();
	if (nullptr == World)
		return;

	UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
	if (nullptr == Tablesystem)
		return;

	FNPCTableRow* NPCTable = Tablesystem->FindTableRow<FNPCTableRow>(TABLE_NAME::NPC, SpawnZoneData.CharacterTID);
	if (nullptr == NPCTable)
		return;

	TSubclassOf<ACC_NPCCharacter> npc = NPCTable->NPCClass;
	
	if (nullptr == npc)
	{
		UE_LOG(LogTemp, Error, TEXT("스폰 실패: TID '%s'의 데이터 테이블에 NPCClass가 None으로 비어있습니다!"), *SpawnZoneData.CharacterTID.ToString());
		return;
	}

	ACC_NPCCharacter* NPC = Comp->GetWorld()->SpawnActor<ACC_NPCCharacter>(npc, Location, Rotation);

	if (NPC)
	{
		NPC->SetNPCData(SpawnZoneData.CharacterTID);
	}

	SetupCharacter(NPC);
}


bool UActiveSpawnPoint::TryGetValidSpawnLocation(const FVector& ProposedLocation, FVector& OutValidLocation)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSys)
	{
		FNavLocation ProjectedLocation;
		// NavMesh 위 유효한 지점 탐색
		if (NavSys->ProjectPointToNavigation(ProposedLocation, ProjectedLocation, FVector(50.0f, 50.0f, 500.0f)))
		{
			// NavMesh에 투영된 최종 위치를 출력 매개변수에 저장
			OutValidLocation = ProjectedLocation.Location;
			return true;
		}
		else
		{
			//(유효하지 않은 위치)
			return false;
		}
	}
	// NavSys가 없다면 (NavMesh 미사용 시) 검사를 건너뛰고 위치를 그대로 사용하도록 함
	// 만약 NavSys가 필수라면 여기서 return false; 처리가 가능함
	// 현재는 NavSys가 없으면 검사 없이 원래 위치를 유효하다고 간주
	OutValidLocation = ProposedLocation;
	return true;
}

void UActiveSpawnPoint::On_RepMonsterCountReset()
{

}

void UActiveSpawnPoint::server_RequestSpawnAction_Implementation()
{

	switch (Type)
	{
	case eSpawnModeType::SINGLE_POINT:
		SinglePoint_SpawnAction();
		break;
	case eSpawnModeType::RANDOM_CIRCLE:
		RandomCircle_SpawnAction();
		break;
	case eSpawnModeType::RANDOM_RECTANGLE:
		RandomRectangle_SpawnAction();
		break;

	default:
		break;
	}
}

void UActiveSpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UActiveSpawnPoint, SpawnCharacters);
}
