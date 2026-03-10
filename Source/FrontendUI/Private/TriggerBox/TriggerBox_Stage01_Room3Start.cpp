// Vince Petrelli All Rights Reserved


#include "TriggerBox/TriggerBox_Stage01_Room3Start.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/CC_PlayerCharacter.h"


ATriggerBox_Stage01_Room3Start::ATriggerBox_Stage01_Room3Start()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void ATriggerBox_Stage01_Room3Start::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &ATriggerBox_Stage01_Room3Start::OnOverlapBegin);
}

void ATriggerBox_Stage01_Room3Start::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsRotating)
    {
        PlayerCameraAngleChange(DeltaTime);
    }
}


void ATriggerBox_Stage01_Room3Start::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
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
        if (CameraArmRot.Equals(FRotator(-20.0f, 0.0f, 0.0f), 1.0f))
        {
            player->SetDashCostMana(30.f);
            TargetPlayer = player;
            CameraArm->bUsePawnControlRotation = false;
            bIsRotating = true;
        }
    }
}

void ATriggerBox_Stage01_Room3Start::PlayerCameraAngleChange(float DeltaTime)
{
    if (bIsRotating && TargetPlayer)
    {
        USpringArmComponent* CameraArm = TargetPlayer->GetSpringArm();
        if (CameraArm)
        {
            FRotator CurrentRot = CameraArm->GetRelativeRotation();
            FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, DeltaTime, 2.0f);
            CameraArm->SetRelativeRotation(NewRot);

            float CurrentLength = CameraArm->TargetArmLength;
            float NewLength = FMath::FInterpTo(CurrentLength, TargetArmLength, DeltaTime, 5.0f);
            CameraArm->TargetArmLength = NewLength;

            bool bRotReached = NewRot.Equals(TargetRotation, 0.1f);
            bool bLenReached = FMath::IsNearlyEqual(NewLength, TargetArmLength, 1.0f);

            // 목표 각도에 거의 도달했다면 보간 중단
            if (bRotReached && bLenReached)
            {
                CameraArm->SetRelativeRotation(TargetRotation);
                CameraArm->TargetArmLength = TargetArmLength;

                bIsRotating = false;
                TargetPlayer = nullptr;
                UE_LOG(LogTemp, Log, TEXT("카메라 회전 완료"));
            }
        }
    }
}