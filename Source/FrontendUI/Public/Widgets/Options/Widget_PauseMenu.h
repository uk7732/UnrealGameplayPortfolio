// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_PauseMenu.generated.h"

class UFrontendCommonButtonBase;

UCLASS()
class FRONTENDUI_API UWidget_PauseMenu : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	// CommonUI 인터페이스: 위젯이 활성화(화면에 뜸)될 때 호출
	virtual void NativeOnActivated() override;
	// CommonUI 인터페이스: 위젯이 비활성화(닫힘)될 때 호출
	virtual void NativeOnDeactivated() override;

	// ESC 키 등으로 메뉴를 닫으려 할 때 호출되는 함수 (CommonUI 기본 동작)
	virtual bool NativeOnHandleBackAction() override;

private:
	// 1. 계속하기 (Resume)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Resume;

	// 2. 재시작 (Restart)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Restart;

	// 3. 로비로 이동 (Lobby)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Lobby;

	// 4. 게임 종료 (Exit)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Quit;

	// 버튼 클릭 핸들러
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnLobbyClicked();

	UFUNCTION()
	void OnQuitClicked();
};