// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerBox_Stage01_Room3_Door.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ATriggerBox_Stage01_Room3_Door : public ATriggerBox
{
	GENERATED_BODY()
public:
	ATriggerBox_Stage01_Room3_Door();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<AActor*> OverlappedPlayers;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);


	UFUNCTION(BlueprintImplementableEvent)
	void OpenTheDoor();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseTheDoor();
};
