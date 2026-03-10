// Vince Petrelli All Rights Reserved

#include "Controllers/CC_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Subsytems/FrontendUISubsystem.h" 
#include "Instance/Admin4_GameInstance.h"  
#include "Widgets/Widget_ActivatableBase.h" 
#include "FrontendTypes/FrontendEnumTypes.h" 
#include "FrontendGameplayTags.h"
#include "GameFramework/PlayerState.h" 
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "FrontendFunctionLibrary.h"
#include "Widgets/InGameWidgetLayout.h"
#include "Characters/CC_NPCCharacter.h"

void ACC_PlayerController::ToggleInventory()
{
	// 1. 위젯이 없으면 생성
	if (!InventoryWidget && InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		InventoryWidget->AddToViewport(); // 처음에 미리 붙여둡니다.
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed); // 숨김 상태로 시작
	}

	if (InventoryWidget)
	{
		if (InventoryWidget->IsVisible())
		{
			// [끄기 로직]
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);
			SetInputMode(Mode);

			bShowMouseCursor = true;

			if (FSlateApplication::IsInitialized())
			{
				FSlateApplication::Get().SetAllUserFocusToGameViewport();
			}
		}
		else
		{
			// [켜기 로직]
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);

			FInputModeGameAndUI UIMode;
			// TakeWidget() 대신 GetCachedWidget()이나 위젯 자체를 활용하도록 엔진이 유도함
			// 여기서는 안전하게 위젯의 포커스 설정을 수동으로 잡아줍니다.
			UIMode.SetWidgetToFocus(InventoryWidget->GetCachedWidget());
			UIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(UIMode);

			// 위젯 내부로 포커스 강제 이동
			InventoryWidget->SetFocus();
		}
	}




}

UWidget_InGameActivatableWidget* ACC_PlayerController::PushContentWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	// 1. 메인 레이아웃(토목)이 잘 살아있는지 확인
	if (MainLayoutInstance)
	{
		// 2. 메인 레이아웃 안에 있는 함수를 호출해서 실제 스택에 쌓음
		// (이 함수도 MainLayout에 우리가 만들어둔 것)
		return Cast<UWidget_InGameActivatableWidget>(MainLayoutInstance->PushWidgetToStack(WidgetClass));
	}
	else
	{
		// 디버깅용 로그 (C++ 개발할 때 중요!)
		UE_LOG(LogTemp, Warning, TEXT("메인 레이아웃이 없습니다! Push 실패"));
		return nullptr;
	}
}

void ACC_PlayerController::InteractionPushUI(TSubclassOf<UCommonActivatableWidget> NewWidgetClass)
{
	if (MainLayoutInstance)
	{
		// 메인 레이아웃의 함수를 호출하여 스택에 위젯을 쌓음
		MainLayoutInstance->PushWidgetToStack(NewWidgetClass);
	}
	else
	{
		// 디버깅용 로그 (C++ 개발할 때 중요!)
		UE_LOG(LogTemp, Warning, TEXT("메인 레이아웃이 없습니다! Push 실패"));
	}
}

void ACC_PlayerController::Client_OnInteractWithNPC_Implementation(ACC_NPCCharacter* InteractedNPC,  const FGameplayTag& WidgetTags)
{
	if (!InteractedNPC)
		return;

	TSoftClassPtr<UWidget_InGameActivatableWidget> SoftWidgetClass = InteractedNPC->GetWidgetClass(WidgetTags);

	TSubclassOf< UWidget_InGameActivatableWidget> WidgetClass = SoftWidgetClass.LoadSynchronous();

	if (WidgetClass)
	{

		UWidget_InGameActivatableWidget* CreatedWidget = PushContentWidget(WidgetClass);
		
		if (CreatedWidget)
		{
			CreatedWidget->Init(InteractedNPC);
		}
	}
}


void ACC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);

	// 게임과 UI 모두에 입력을 허용하는 모드로 변경
	FInputModeGameAndUI InputMode;

	// 마우스 입력시 커서 숨기기 끄기
	InputMode.SetHideCursorDuringCapture(false);

	// 마우스를 뷰포트 밖으로 나가지 않도록 잠금 (옵션)
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	// 
	// 마우스 커서를 포함한 입력 모드를 설정
	SetInputMode(InputMode);	

	// 현재 카메라가 보고 있는 대상 (View Target)을 가져옵니다.
	AActor* CurrentViewTarget = GetViewTarget();

	if (CurrentViewTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController: 현재 ViewTarget: %s"), *CurrentViewTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController: ViewTarget이 유효하지 않습니다. (NULL)"));
	}

	// 로컬 플레이어(내 화면)인 경우에만 위젯 생성
	if (IsLocalController())
	{
		// 1. HUD 생성
		if (HUDWidgetClass)
		{
			HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
			if (HUDWidget)
			{
				HUDWidget->AddToViewport(0);
			}
		}

		// 2. 뼈대가 되는 메인 레이아웃 생성 (여기서 딱 한 번만!)
		if (MainLayoutClass && !MainLayoutInstance)
		{
			MainLayoutInstance = CreateWidget<UInGameWidgetLayout>(this, MainLayoutClass);
			if (MainLayoutInstance)
			{
				MainLayoutInstance->AddToViewport(100);
				// 필요하다면 여기서 초기 ZOrder를 설정해서 HUD 밑에 깔리게 하거나 
				// 투명한 상태로 유지할 수 있습니다.
			}
		}
	}
}

void ACC_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UpdateMouseTarget();
}

void ACC_PlayerController::UpdateMouseTarget()
{
	FVector WorldLocation, WorldDirection;
	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FVector Start = WorldLocation;
		FVector End = Start + WorldDirection * 10000.f;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(MouseOcclusionIgnoreActors);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			LastMouseHoverActor = Hit.GetActor();
		}
		else
		{
			LastMouseHoverActor = nullptr;
		}
	}
}

void ACC_PlayerController::server_ReplicateRotUpdate_Implementation(FC_RepRotUpdate rotUpdate)
{
	SetControlRotation(rotUpdate.Rot);
}

void ACC_PlayerController::server_StartMoveAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->MoveAction(Value);
}

void ACC_PlayerController::server_StopMoveAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->MoveStopAction(Value);
}

void ACC_PlayerController::server_StartSprintAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StartSprintingAction();

}

void ACC_PlayerController::server_StopSprintAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StopSprintingAction();
}

void ACC_PlayerController::server_StartDashAction_Implementation()
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StartDashAction();
}

void ACC_PlayerController::server_StopDashAction_Implementation()
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StopDashAction();
}

void ACC_PlayerController::server_ForceStopSprint_Implementation()
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StopSprintingAction();
}

void ACC_PlayerController::server_NormalSkillAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->NormalAction(Value);
}

void ACC_PlayerController::server_SubNormalSkillAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->SubNormalAction(Value);
}


void ACC_PlayerController::server_ReloadAction_Implementation(const FInputActionValue& Value)
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->ReloadAction(Value);
}

void ACC_PlayerController::server_Jump_Implementation()
{

	// 서버에서 실행됨
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player) return;

	// 캐릭터의 스킬 액션 실행 (여기서 쿨타임, 마나 체크 및 몽타주 재생됨)
	// PacketDatas.h에 정의된 FS_ReqSkillCast 구조체의 TID를 사용	
	player->Jump();

}

void ACC_PlayerController::server_StopJumping_Implementation()
{
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player)
		return;

	player->StopJumping();
}

void ACC_PlayerController::server_RequestSkillCast_Implementation(FS_ReqSkillCast skillCast)
{
	// 서버에서 실행됨
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetPawn());
	if (nullptr == player) return;

	// 캐릭터의 스킬 액션 실행 (여기서 쿨타임, 마나 체크 및 몽타주 재생됨)
	// PacketDatas.h에 정의된 FS_ReqSkillCast 구조체의 TID를 사용
	player->SkillAction(skillCast.TID);
}

void ACC_PlayerController::SendChatMessage(const FString& Message)
{
	if (Message.IsEmpty()) return;

	// 서버로 전송
	Server_SendChatMessage(Message);
}

void ACC_PlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	// 보낸 사람 이름 가져오기
	FString SenderName = TEXT("Unknown");
	if (PlayerState)
	{
		SenderName = PlayerState->GetPlayerName();
	}

	// 현재 월드에 있는 모든 플레이어 컨트롤러를 찾아서 메시지 전송
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACC_PlayerController* PC = Cast<ACC_PlayerController>(It->Get());
		if (PC)
		{
			PC->Client_ReceiveChatMessage(SenderName, Message);
		}
	}
}

void ACC_PlayerController::Client_ReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	// UI에게 알림 (WBP_Chat 위젯이 이 이벤트를 듣고 있어야 함)
	OnChatReceived.Broadcast(SenderName, Message);
}

void ACC_PlayerController::Server_ExecuteLevelTransfer_Implementation(const FName& LevelName)
{
	UTableSubsystem* tablesystem = UTableSubsystem::Get(this);
	if (nullptr == tablesystem)
		return;

	FMapTableRow* MapTable = tablesystem->FindTableRow<FMapTableRow>(TABLE_NAME::MAP, LevelName);
	if (nullptr == MapTable)
		return;

	FString MapPath = MapTable->MapPath;


	UAdmin4_GameInstance* GameInst = Cast<UAdmin4_GameInstance>(GetGameInstance());
	if (GameInst)
	{
		GameInst->NextMap(MapPath);
	}
}

void ACC_PlayerController::Client_ShowMapSelectionScreen_Implementation()
{
	if (UFrontendUISubsystem* UISys = UFrontendUISubsystem::Get(this))
	{
		// 모달 스택에 맵 선택 위젯 푸시
		// (GameplayTags에 Frontend_Widget_MapSelectionScreen 추가 필요)
		UISys->PushSoftWidgetToStackAsync(
			FrontendGameplayTags::Frontend_WidgetStack_Modal,
			UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_MapSelectionScreen), // 이 태그 등록 필요
			[](EAsyncPushWidgetState, UWidget_ActivatableBase*) {}
		);
	}
}

void ACC_PlayerController::Client_ShowLevelTransferConfirm_Implementation(const FName& LevelName)
{
	// 로컬 플레이어만 UI를 볼 수 있음
	if (!IsLocalController()) return;

	UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(this);
	if (!UISubsystem) return;

	// 팝업에 표시할 텍스트 구성
	FText Title = FText::FromString(TEXT("Level Transfer"));
	FText Message = FText::Format(FText::FromString(TEXT("Do you want to travel to {0}?")), FText::FromName(LevelName));

	// 기존에 구현된 Confirm Screen 시스템 활용 (Yes/No 팝업)
	UISubsystem->PushConfirmScreenToModalStackAynsc(
		EConfirmScreenType::YesNo, // Yes, No 버튼이 있는 타입
		Title,
		Message,
		[this, LevelName](EConfirmScreenButtonType ButtonType)
		{
			// 람다 함수: 버튼 클릭 시 실행됨
			if (ButtonType == EConfirmScreenButtonType::Confirmed) // Yes 버튼 클릭
			{
				// 서버에게 이동 요청
				Server_ExecuteLevelTransfer(LevelName);
			}
			else
			{
				// No 버튼 클릭 시 아무것도 안 함 (팝업은 자동으로 닫힘)
			}
		}
	);
}
