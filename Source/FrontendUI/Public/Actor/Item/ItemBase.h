// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Actor/ActorBase.h"
#include "Common/GameDatas.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API AItemBase : public AActorBase
{
	GENERATED_BODY()
	
public:
	AItemBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystemComponent> ShineEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ItemData)
	FItemData ItemData;

public:
	void InitializedItemData(const FName& ItemTID, int64 InUID, int32 InCount = 1);
private:
	bool bIsPendingDestroy = false;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnRep_ItemData();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void server_InitializedItemData(const FName& ItemTID, int64 InUID, int32 InCount);
	void server_InitializedItemData_Implementation(const FName& ItemTID, int64 InUID, int32 InCount);

};
