// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Common/PacketDatas.h"

#include "CC_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatReceived, const FString&, SenderName, const FString&, Message);

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCommonActivatableWidget;
class UInGameWidgetLayout;
class ACC_NPCCharacter;
class UWidget_InGameActivatableWidget;

UCLASS()
class FRONTENDUI_API ACC_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:	
	// 에디터에서 WBP_MainHUD를 지정할 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;


	// 생성된 위젯을 담을 변수
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> HUDWidget;

	// 인벤토리 위젯 클래스 (BP에서 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryWidgetClass;

	// 인벤토리 위젯 인스턴스
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> InventoryWidget;
	// 마우스 무시용
	UPROPERTY()
	TArray<AActor*> MouseOcclusionIgnoreActors;
	// ACC_PlayerController.h
	UPROPERTY(BlueprintReadOnly, Category = "Mouse")
	AActor* LastMouseHoverActor;

	// 인벤토리 열기/닫기 함수
	void ToggleInventory();

	UWidget_InGameActivatableWidget* PushContentWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	// 상호작용 시 호출: "어떤 위젯 클래스"를 띄울지 매개변수로 받음
	void InteractionPushUI(TSubclassOf<UCommonActivatableWidget> NewWidgetClass);
	
	UFUNCTION(Client, Reliable)
	void Client_OnInteractWithNPC(ACC_NPCCharacter* InteractedNPC, const FGameplayTag& WidgetTags);
	void Client_OnInteractWithNPC_Implementation(ACC_NPCCharacter* InteractedNPC, const FGameplayTag& WidgetTags);

	// 채팅 포커스 함수
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Chat")
	void FocusChat();
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

	void UpdateMouseTarget();

	// [멤버 변수] 토목 역할을 하는 레이아웃 위젯
	UPROPERTY()
	UInGameWidgetLayout* MainLayoutInstance;

	// [설계도] 에디터에서 할당할 레이아웃 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UInGameWidgetLayout> MainLayoutClass;


public:

	UFUNCTION(Client, Reliable)
	void Client_ShowMapSelectionScreen();
	void Client_ShowMapSelectionScreen_Implementation();

	// 1. 서버(NPC)가 클라이언트에게 "레벨 이동 할래?" 팝업을 띄우라고 명령하는 함수
	UFUNCTION(Client, Reliable)
	void Client_ShowLevelTransferConfirm(const FName& LevelName);
	void Client_ShowLevelTransferConfirm_Implementation(const FName& LevelName);

	// 2. 클라이언트가 "Yes"를 눌렀을 때 서버에게 "진짜 이동시켜줘"라고 요청하는 함수
	UFUNCTION(Server, Reliable)
	void Server_ExecuteLevelTransfer(const FName& LevelName);
	void Server_ExecuteLevelTransfer_Implementation(const FName& LevelName);

	//==========Server RPC ===========

	UFUNCTION(Server, Reliable)
	void server_ReplicateRotUpdate(FC_RepRotUpdate rotUpdate);
	void server_ReplicateRotUpdate_Implementation(FC_RepRotUpdate rotUpdate);


	UFUNCTION(Server, Reliable)
	void server_StartMoveAction(const FInputActionValue& Value);
	void server_StartMoveAction_Implementation(const FInputActionValue& Value);


	UFUNCTION(Server, Reliable)
	void server_StopMoveAction(const FInputActionValue& Value);
	void server_StopMoveAction_Implementation(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void server_StartSprintAction(const FInputActionValue& Value);
	void server_StartSprintAction_Implementation(const FInputActionValue& Value);


	UFUNCTION(Server, Reliable)
	void server_StopSprintAction(const FInputActionValue& Value);
	void server_StopSprintAction_Implementation(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void server_StartDashAction();
	void server_StartDashAction_Implementation();

	UFUNCTION(Server, Reliable)
	void server_StopDashAction();
	void server_StopDashAction_Implementation();

	//매핑값 없이 SprintAction 멈추는 함수
	UFUNCTION(Server, Reliable)
	void server_ForceStopSprint();
	void server_ForceStopSprint_Implementation();

	UFUNCTION(Server, Reliable)
	void server_NormalSkillAction(const FInputActionValue& Value);
	void server_NormalSkillAction_Implementation(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void server_SubNormalSkillAction(const FInputActionValue& Value);
	void server_SubNormalSkillAction_Implementation(const FInputActionValue& Value);




	UFUNCTION(Server, Reliable)
	void server_ReloadAction(const FInputActionValue& Value);
	void server_ReloadAction_Implementation(const FInputActionValue& Value);



	UFUNCTION(Server, Reliable)
	void server_Jump();
	void server_Jump_Implementation();

	UFUNCTION(Server, Reliable)
	void server_StopJumping();
	void server_StopJumping_Implementation();

	UFUNCTION(Server, Reliable)
	void server_RequestSkillCast(FS_ReqSkillCast skillCast);
	void server_RequestSkillCast_Implementation(FS_ReqSkillCast skillCast);


	// 채팅 
	// UI에서 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FOnChatReceived OnChatReceived;

	// UI(WBP_Chat)에서 호출할 함수: 메시지 전송 요청
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendChatMessage(const FString& Message);

	// 서버: 메시지를 받아 모든 클라이언트에게 뿌림
	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FString& Message);
	void Server_SendChatMessage_Implementation(const FString& Message);

	// 클라이언트: 서버로부터 메시지를 받아 UI 델리게이트 실행
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FString& SenderName, const FString& Message);	
	void Client_ReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message);	
};
