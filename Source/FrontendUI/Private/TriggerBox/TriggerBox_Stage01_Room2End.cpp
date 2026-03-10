// Vince Petrelli All Rights Reserved


#include "TriggerBox/TriggerBox_Stage01_Room2End.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/CC_PlayerCharacter.h"



ATriggerBox_Stage01_Room2End::ATriggerBox_Stage01_Room2End()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}
void ATriggerBox_Stage01_Room2End::BeginPlay()
{
    Super::BeginPlay(); 

    OnActorBeginOverlap.AddDynamic(this, &ATriggerBox_Stage01_Room2End::OnOverlapBegin);
}

void ATriggerBox_Stage01_Room2End::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 

    if (bIsRotating)
    {
        PlayerCameraAngleChange(DeltaTime);
    }
}

void ATriggerBox_Stage01_Room2End::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

    if (false == OtherActor->ActorHasTag(TEXT("Player")))
        return;

    ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(OtherActor);
    if (nullptr == player)
        return;

    USpringArmComponent* CameraArm = player->GetSpringArm();
    if (nullptr == CameraArm)
        return;

    if (player && player->IsLocallyControlled())
    {
        FRotator CameraArmRot = CameraArm->GetRelativeRotation();
        if (CameraArmRot.Equals(FRotator(-45.0f, -45.0f, 0), 1.0f))
        {
            player->SetDashCostMana(10.f);
            TargetPlayer = player;
            CameraArm->bUsePawnControlRotation = false;
            bIsRotating = true;
        }
    }
}

void ATriggerBox_Stage01_Room2End::PlayerCameraAngleChange(float DeltaTime)
{
    if (bIsRotating && TargetPlayer)
    {
        USpringArmComponent* CameraArm = TargetPlayer->GetSpringArm();
        if (CameraArm)
        {
            FRotator CurrentRot = CameraArm->GetRelativeRotation();

            // FMath::RInterpTo(현재값, 목표값, 델타타임, 속도)
            FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, DeltaTime, 2.0f);

            CameraArm->SetRelativeRotation(NewRot);

            // 목표 각도에 거의 도달했다면 보간 중단
            if (NewRot.Equals(TargetRotation, 0.1f))
            {
                bIsRotating = false;
                TargetPlayer = nullptr;
                UE_LOG(LogTemp, Log, TEXT("카메라 회전 완료"));
            }
        }
    }
}
