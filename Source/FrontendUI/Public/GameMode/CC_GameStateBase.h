// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CC_GameStateBase.generated.h"

// (파라미터로 열릴 방 번호를 전달)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomOpened, int32, RoomNum);

/**
 *
 */
UCLASS()
class ACC_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACC_GameStateBase();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 2. 블루프린트에서 호출을 받을 수 있도록 Visible/BlueprintAssignable 설정
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRoomOpened OnRoomOpened;

	UPROPERTY(ReplicatedUsing = OnRep_IsOpenRoom, BlueprintReadOnly)
	TArray<bool> IsOpenRoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> PrevOpenRoom;

	UPROPERTY(Replicated)
	TArray<struct FRoomData> RoomData;

	UFUNCTION()
	void OnRep_IsOpenRoom();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Open")
	void OpentheRoom(int32 RoomNum);
};
