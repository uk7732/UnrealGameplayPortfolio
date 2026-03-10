// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerBox_Stage01_Room3Start.generated.h"

/**
 * 
 */
UCLASS()
class ATriggerBox_Stage01_Room3Start : public ATriggerBox
{
	GENERATED_BODY()
	
public:
    ATriggerBox_Stage01_Room3Start();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;


    UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

    void PlayerCameraAngleChange(float DeltaTime);

private:
    bool bIsRotating = false;
    class ACC_PlayerCharacter* TargetPlayer = nullptr;
    FRotator TargetRotation = FRotator(-45.f, -45.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Camera")
    float TargetArmLength = 1700.0f;

};
