// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnManagerComponent.generated.h"

class UActiveSpawnPoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTENDUI_API USpawnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region SpawnManagerComponent
	void InitActiveSpawnPoint(const FName& SpawnTID);
	void StartTriggerSpawn(const FName& SpawnName);
private:
	FTimerHandle SpawnTimeHandle;
	float TimeCycle = 5.0;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = "SpawnData");
	TArray<UActiveSpawnPoint*> SpawnDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = "SpawnData");
	TArray<UActiveSpawnPoint*> TriggerSpawnData;

	void SpawnDatasTimeAction();

private:
	UFUNCTION(Server, Reliable)
	void server_InitActiveSpawnPoint(const FName& SpawnGroupTID);
	void server_InitActiveSpawnPoint_Implementation(const FName& SpawnGroupTID);

	UFUNCTION(Server, Reliable)
	void server_StartTriggerSpawn(const FName& TriggerName);
	void server_StartTriggerSpawn_Implementation(const FName& TriggerName);

#pragma endregion

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
