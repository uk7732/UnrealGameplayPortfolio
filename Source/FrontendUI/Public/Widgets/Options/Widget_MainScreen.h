// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_MainScreen.generated.h"

class UFrontendCommonButtonBase;
class UEditableTextBox; 

UCLASS()
class FRONTENDUI_API UWidget_MainScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	// [중요] 사진 속 위젯 이름과 변수명이 100% 일치해야 합니다!

	// 1. 싱글 플레이용 버튼 (NEW GAME)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)		
	TObjectPtr<UFrontendCommonButtonBase> Button_Single;

	// 2. 멀티 호스트용 버튼 (CONTINUE로 대체해서 사용 예시)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UFrontendCommonButtonBase> Button_Host;

	// 3. 멀티 참가용 버튼 (NEW GAME+로 대체해서 사용 예시)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UFrontendCommonButtonBase> Button_JoinHost;

	// 4. IP 입력창 (사진에는 없지만, 멀티 참가를 위해 추가 필요)
	// WBP에 추가하고 이름을 Edit_IPAddress로 지어야 함
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UEditableTextBox> Edit_IPAddress;

	// 에디터에서 WBP_JoinGame을 지정해줘야 함
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UWidget_ActivatableBase> JoinGameWidgetClass;

	// 이동할 레벨 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	FName GameplayLevelName = FName("Lobby");

	// 클릭 핸들러
	void OnSinglePlayClicked();
	void OnHostGameClicked();
	void OnJoinGameClicked();
};
