// Vince Petrelli All Rights Reserved


#include "TriggerBox/TriggerBox_Stage01_Room3_Door.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Components/BoxComponent.h"

ATriggerBox_Stage01_Room3_Door::ATriggerBox_Stage01_Room3_Door()
{

}

void ATriggerBox_Stage01_Room3_Door::BeginPlay()
{
	Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &ATriggerBox_Stage01_Room3_Door::OnOverlapBegin);
    OnActorEndOverlap.AddDynamic(this, &ATriggerBox_Stage01_Room3_Door::OnOverlapEnd);
}

void ATriggerBox_Stage01_Room3_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ATriggerBox_Stage01_Room3_Door::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

    if (!HasAuthority() || OtherActor == nullptr || OtherActor == this)
        return;

    if (OtherActor && OtherActor->IsA(ACC_PlayerCharacter::StaticClass()))
    {
        OverlappedPlayers.AddUnique(OtherActor);

        UE_LOG(LogTemp, Warning, TEXT("Current Overlapped Players: %d"), OverlappedPlayers.Num());

        if (OverlappedPlayers.Num() >= 1)
        {
            OpenTheDoor();
        }
    }
}

void ATriggerBox_Stage01_Room3_Door::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!HasAuthority() || OtherActor == nullptr || OtherActor == this)
        return;

    if (OtherActor && OtherActor->IsA(ACC_PlayerCharacter::StaticClass()))
    {
        OverlappedPlayers.Remove(OtherActor);

        if (OverlappedPlayers.Num() < 1)
        {
            CloseTheDoor();
        }
    }
}
