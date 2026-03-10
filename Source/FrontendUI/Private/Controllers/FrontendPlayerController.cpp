// Vince Petrelli All Rights Reserved


#include "Controllers/FrontendPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Subsytems/FrontendUISubsystem.h" 
#include "FrontendGameplayTags.h"          

void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,ACameraActor::StaticClass(),FName("Default"),FoundCameras);

	if (!FoundCameras.IsEmpty())
	{
		SetViewTarget(FoundCameras[0]);
	}
	UFrontendGameUserSettings* GameUserSettings = UFrontendGameUserSettings::Get();

	if (GameUserSettings->GetLastCPUBenchmarkResult() == -1.f || GameUserSettings->GetLastGPUBenchmarkResult())
	{
		GameUserSettings->RunHardwareBenchmark();
		GameUserSettings->ApplyHardwareBenchmarkResults();
	}
}

void AFrontendPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Enhanced Input을 사용하신다면 해당 방식으로 바인딩하시고,
	// 여기서는 레거시 방식으로 예시를 듭니다. 
	// 프로젝트 세팅 -> Input -> Action Mappings에 "Pause" (Key: Escape)가 있다고 가정
	InputComponent->BindAction("Pause", IE_Pressed, this, &ThisClass::RequestPauseMenu);
}

void AFrontendPlayerController::RequestPauseMenu()
{
	// 이미 일시정지 상태라면 처리를 UI에게 맡기거나 무시
	if (IsPaused())
	{
		return;
	}

	if (PauseMenuWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("PauseMenuWidgetClass is not set in PlayerController!"));
		return;
	}

	// UI 서브시스템을 통해 위젯 스택에 PauseMenu를 Push합니다.
	if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(GetLocalPlayer()))
	{
		// FrontendGameplayTags::Frontend_WidgetStack_GameMenu 태그를 사용하거나
		// 기존에 정의된 Modal Stack 태그를 사용하세요.
		UISubsystem->PushSoftWidgetToStackAsync(
			FrontendGameplayTags::Frontend_WidgetStack_Modal,
			PauseMenuWidgetClass,
			[](EAsyncPushWidgetState, UWidget_ActivatableBase*) {}
		);
	}
}