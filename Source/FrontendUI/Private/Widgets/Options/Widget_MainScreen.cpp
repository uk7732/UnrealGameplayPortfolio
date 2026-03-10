// Vince Petrelli All Rights Reserved


#include "Widgets/Options/Widget_MainScreen.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Components/EditableTextBox.h"
#include "Instance/Admin4_GameInstance.h" 
#include "FrontendGameplayTags.h" 
#include "Subsytems/FrontendUISubsystem.h" 
#include "Widgets/Widget_ActivatableBase.h"

void UWidget_MainScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 버튼 이벤트 연결
	if (Button_Single)
	{
		Button_Single->OnClicked().AddUObject(this, &ThisClass::OnSinglePlayClicked);
	}
	if (Button_Host)
	{
		// 예시: Continue 버튼을 Host 버튼으로 사용
		Button_Host->OnClicked().AddUObject(this, &ThisClass::OnHostGameClicked);
		Button_Host->SetIsEnabled(true);
	}
	if (Button_JoinHost)
	{
		// 예시: NewGame+ 버튼을 Join 버튼으로 사용
		Button_JoinHost->OnClicked().AddUObject(this, &ThisClass::OnJoinGameClicked);
		Button_JoinHost->SetIsEnabled(true);
	}
}

void UWidget_MainScreen::OnSinglePlayClicked()
{
	if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetGameInstance()))
	{
		// 싱글 플레이로 레벨 이동
		GI->LaunchSingleplayer(GameplayLevelName);
	}
}

void UWidget_MainScreen::OnHostGameClicked()
{
	if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetGameInstance()))
	{
		// 리슨 서버로 레벨 열기
		GI->HostMultiplayer(GameplayLevelName);
	}
}

void UWidget_MainScreen::OnJoinGameClicked()
{
	if (JoinGameWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("JoinGameWidgetClass가 지정되지 않았습니다! WBP에서 설정해주세요."));
		return;
	}

	// Modal Stack(팝업 레이어)에 위젯을 Push
	UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal, // 모달 스택 태그
		JoinGameWidgetClass,
		[](EAsyncPushWidgetState, UWidget_ActivatableBase*) {}
	);
}