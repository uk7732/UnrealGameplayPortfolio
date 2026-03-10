// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ConfirmPopup.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class FRONTENDUI_API UWidget_ConfirmPopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//외부에서 팝업을 띄울 떄 이동할 맵 정보를 전달해주는 함수
	void SetupPopup(FName InTargetMapTID, FText InMapName);

protected:
	//가상함수 오버라이드
	virtual void NativeConstruct() override;


private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Message;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Yes;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_No;

	FName TargetMapTID;

	UFUNCTION()
	void OnClick_Yes();

	UFUNCTION()
	void OnClick_No();
};
