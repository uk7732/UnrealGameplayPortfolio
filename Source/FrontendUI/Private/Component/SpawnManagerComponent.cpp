// Vince Petrelli All Rights Reserved


#include "Component/SpawnManagerComponent.h"
#include "Spawn/ActiveSpawnPoint.h"
#include "Instance/TableSubsystem.h"
#include "Net/UnrealNetwork.h"



USpawnManagerComponent::USpawnManagerComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	// ...
}



void USpawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();


	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimeHandle,
		this,
		&USpawnManagerComponent::SpawnDatasTimeAction,
		TimeCycle,   // 5초마다 계산
		true
	);

}

void USpawnManagerComponent::SpawnDatasTimeAction()
{
	for (auto& it : SpawnDatas)
	{
		if (it->GetSpawnInterval() == 0)
			continue;

		if (it->IsIntervalTimeCheck(TimeCycle))
		{
			it->RequestSpawnAction();
			UE_LOG(LogTemp, Warning, TEXT("IntervalCheck: %s"), it->IsIntervalTimeCheck(TimeCycle) ? TEXT("True") : TEXT("False"));
		}
	}
}

void USpawnManagerComponent::InitActiveSpawnPoint(const FName& SpawnTID)
{
	server_InitActiveSpawnPoint(SpawnTID);
}

void USpawnManagerComponent::StartTriggerSpawn(const FName& SpawnName)
{
	server_StartTriggerSpawn(SpawnName);
}


void USpawnManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USpawnManagerComponent::server_InitActiveSpawnPoint_Implementation(const FName& SpawnGroupTID)
{
	UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
	if (nullptr == tablesystem)
		return;


	FSpawnGroupTableRow* SpawnGroupTable = tablesystem->FindTableRow<FSpawnGroupTableRow>(TABLE_NAME::SPAWNGROUP, SpawnGroupTID);
	if (nullptr == SpawnGroupTable)
		return;


	for (auto& it : SpawnGroupTable->SpawnTableTIDs)
	{
		UActiveSpawnPoint* ActiveSpawnPoint = NewObject<UActiveSpawnPoint>(this);
		ActiveSpawnPoint->InitializerActiveSpawnPoint(it);
		
		if(ActiveSpawnPoint->GetIsTriggerSpawnType())
			TriggerSpawnData.Add(ActiveSpawnPoint);
		else
			SpawnDatas.Add(ActiveSpawnPoint);
	}

	for (auto& it : SpawnDatas)
	{
		it->RequestSpawnAction();
		UE_LOG(LogTemp, Warning, TEXT("IntervalCheck: %s"), it->IsIntervalTimeCheck(TimeCycle) ? TEXT("첫스폰 성공") : TEXT("첫스폰 실패"));
	}
}

void USpawnManagerComponent::server_StartTriggerSpawn_Implementation(const FName& TriggerName)
{
	for (auto& it : TriggerSpawnData)
	{
		if (TriggerName == it->GetSpawnName())
			it->RequestSpawnAction();
	}
}

void USpawnManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USpawnManagerComponent, SpawnDatas);
	DOREPLIFETIME(USpawnManagerComponent, TriggerSpawnData);
}

