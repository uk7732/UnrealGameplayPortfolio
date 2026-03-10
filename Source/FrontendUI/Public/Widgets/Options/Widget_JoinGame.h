// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_JoinGame.generated.h"

class UFrontendCommonButtonBase;
class UEditableText;


UCLASS()
class FRONTENDUI_API UWidget_JoinGame : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	// CommonUI가 활성화될 때 포커스 대상을 묻는 함수
	virtual UWidget* NativeGetDesiredFocusTarget() const override;


private:
	// IP 입력칸
	UPROPERTY(meta = (BindWidget))	
	TObjectPtr<UEditableText> Edit_IPAddress;

	// 접속 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Connect;

	// 취소(뒤로가기) 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Btn_Cancel;

	UFUNCTION()
	void OnConnectClicked();

	UFUNCTION()
	void OnCancelClicked();
};
