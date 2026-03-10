// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerBox_Stage01_Room2End.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ATriggerBox_Stage01_Room2End : public ATriggerBox
{
	GENERATED_BODY()
	

public:
    ATriggerBox_Stage01_Room2End();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;


    UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

    void PlayerCameraAngleChange(float DeltaTime);

private:
    bool bIsRotating = false;
    class ACC_PlayerCharacter* TargetPlayer = nullptr;
    FRotator TargetRotation = FRotator(-20.f, 0.f , 0.f);
};

