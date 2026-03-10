// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Common/PacketDatas.h"
#include "FrontendPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API AFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	//~ Begin APlayerController Interface
	virtual void OnPossess(APawn* aPawn) override;
	//~ End APlayerController Interface

	virtual void SetupInputComponent() override;

	// ESC 입력 처리 함수
	void RequestPauseMenu();

	// Pause 메뉴 위젯 클래스 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<class UWidget_ActivatableBase> PauseMenuWidgetClass;

public:

};
