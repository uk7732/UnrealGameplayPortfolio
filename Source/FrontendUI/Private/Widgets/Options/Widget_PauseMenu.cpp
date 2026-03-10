// Vince Petrelli All Rights Reserved


#include "Widgets/Options/Widget_PauseMenu.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Instance/Admin4_GameInstance.h" 
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

void UWidget_PauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Resume)
	{
		Btn_Resume->OnClicked().AddUObject(this, &ThisClass::OnResumeClicked);
	}
	if (Btn_Restart)
	{
		Btn_Restart->OnClicked().AddUObject(this, &ThisClass::OnRestartClicked);
	}
	if (Btn_Lobby)
	{
		Btn_Lobby->OnClicked().AddUObject(this, &ThisClass::OnLobbyClicked);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked().AddUObject(this, &ThisClass::OnQuitClicked);
	}
}

void UWidget_PauseMenu::NativeOnActivated()
{
	Super::NativeOnActivated();

	// 싱글
	// 메뉴가 뜰 때 게임 일시정지
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}

	// UI 조작을 위한 입력 모드 설정 (멀티/싱글 공통)
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);

		// UI만 조작 가능하게 변경 (게임 캐릭터 조작 방지)
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		if (GetWorld()->GetNetMode() == NM_Client)
		{
			if (Btn_Restart)
			{
				// 버튼을 아예 숨기거나
				//Btn_Restart->SetVisibility(ESlateVisibility::Collapsed);
				// 또는 비활성화
				Btn_Restart->SetIsEnabled(false);
			}
		}
	}
}

void UWidget_PauseMenu::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();


	// 싱글 플레이였다면 시간 정지 해제
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::SetGamePaused(this, false);
	}

	// 게임 조작 모드로 복구 (멀티/싱글 공통)
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

bool UWidget_PauseMenu::NativeOnHandleBackAction()
{
	// ESC 키를 눌렀을 때 Resume 버튼과 동일하게 동작하도록 처리
	OnResumeClicked();
	return true; // 이벤트를 처리했음을 알림
}

void UWidget_PauseMenu::OnResumeClicked()
{
	// 위젯을 비활성화(닫기) -> NativeOnDeactivated 호출됨 -> 게임 재개
	DeactivateWidget();
}

void UWidget_PauseMenu::OnRestartClicked()
{

	// 1. 일시정지 해제 (필수)
	UGameplayStatics::SetGamePaused(this, false);

	UWorld* World = GetWorld();
	if (!World) return;

	// 현재 맵 이름 가져오기
	// (에디터 플레이 시 UEDPIE 접두사가 붙을 수 있어 제거 작업 필요)
	FString CurrentMapName = World->GetMapName();
	CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);

	// 2. 권한 확인 (서버인가?)
	if (World->GetNetMode() == NM_ListenServer || World->GetNetMode() == NM_DedicatedServer)
	{
		// [멀티플레이 호스트] : ServerTravel을 사용하여 모두 함께 재시작
		// "?listen"을 붙여야 리슨 서버 상태가 유지됩니다.
		World->ServerTravel(CurrentMapName + TEXT("?listen"));
	}
	else if (World->GetNetMode() == NM_Client)
	{
		// [멀티플레이 클라이언트] : 권한 없음 (아무것도 안 하거나, 로그 출력)
		// UI에서 이미 숨겼다면 이 코드는 실행될 일 없음
		UE_LOG(LogTemp, Warning, TEXT("클라이언트는 재시작을 요청할 수 없습니다."));
	}
	else
	{
		// [싱글 플레이] : 기존 방식대로 로컬 재시작
		UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
	}
}

void UWidget_PauseMenu::OnLobbyClicked()
{
	// 1. 일시정지 해제
	UGameplayStatics::SetGamePaused(this, false);

	UWorld* World = GetWorld();
	if (!World) return;

	// 이동할 로비 맵 이름 (정확해야 함)
	FString LobbyMapName = TEXT("Lobby"); // 질문자님의 맵 이름

	// 2. 권한 확인
	if (World->GetNetMode() == NM_ListenServer || World->GetNetMode() == NM_DedicatedServer)
	{
		// [멀티플레이 호스트] : 모두 데리고 로비로 이동
		// 로비에서도 다른 유저가 들어와야 한다면 ?listen 유지
		World->ServerTravel(LobbyMapName + TEXT("?listen"));
	}
	else if (World->GetNetMode() == NM_Client)
	{
		// [멀티플레이 클라이언트] : '나 혼자' 나가기
		// 클라이언트는 ServerTravel을 할 수 없으므로, 스스로 연결을 끊고 로비 맵을 엽니다.
		UGameplayStatics::OpenLevel(this, FName(*LobbyMapName));
	}
	else
	{
		// [싱글 플레이] : 로컬 이동
		UGameplayStatics::OpenLevel(this, FName(*LobbyMapName));
	}
}

void UWidget_PauseMenu::OnQuitClicked()
{
	UGameplayStatics::SetGamePaused(this, false);

	// "FrontendTestMap"으로 레벨 이동
	UGameplayStatics::OpenLevel(this, FName("FrontendTestMap"));

	// 게임 종료
	//UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}