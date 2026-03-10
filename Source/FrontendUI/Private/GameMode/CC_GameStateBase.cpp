// Vince Petrelli All Rights Reserved


#include "GameMode/CC_GameStateBase.h"
#include "Instance/TableSubsystem.h"
#include "Component/SpawnManagerComponent.h"
#include "Net/UnrealNetwork.h"

ACC_GameStateBase::ACC_GameStateBase()
{

}

void ACC_GameStateBase::BeginPlay()
{
	Super::BeginPlay();

	//OnRoomOpened.AddDynamic(this, &ACC_GameStateBase::OpentheRoom);
}

void ACC_GameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}


void ACC_GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACC_GameStateBase, RoomData);
	DOREPLIFETIME(ACC_GameStateBase, IsOpenRoom);
}

void ACC_GameStateBase::OnRep_IsOpenRoom()
{

    while (PrevOpenRoom.Num() < IsOpenRoom.Num())
    {
        PrevOpenRoom.Add(false);
    }

    for (int32 i = 0; i < IsOpenRoom.Num(); ++i)
    {
        // 서버에서 온 값은 true인데, 내 로컬(PrevOpenRoom)에서는 아직 false라면? -> 새로 열린 방!
        if (IsOpenRoom.IsValidIndex(i) && PrevOpenRoom.IsValidIndex(i))
        {
            if (IsOpenRoom[i] && !PrevOpenRoom[i])
            {
                PrevOpenRoom[i] = true;
                OnRoomOpened.Broadcast(i);
                OpentheRoom(i);

                UE_LOG(LogTemp, Warning, TEXT("[GameState] OnRep_IsOpenRoom 실행됨! 방 번호: %d"), i);
            }
        }
    }
}
