// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "InGameWidgetLayout.generated.h"


class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
/**
 * 
 */
UCLASS()
class FRONTENDUI_API UInGameWidgetLayout : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
    // 컨트롤러가 호출할 함수: 스택에 위젯을 쌓음
    UFUNCTION(BlueprintCallable, Category = "UI")
    UCommonActivatableWidget* PushWidgetToStack(TSubclassOf<UCommonActivatableWidget> WidgetClass);

protected:
    // 블루프린트에서 "Common Activatable Widget Stack" 위젯을 배치하고 이름을 맞춰야 합니다.
    // meta = (BindWidget)은 C++ 변수와 블루프린트 위젯을 연결해줍니다.
    UPROPERTY(meta = (BindWidget))
    UCommonActivatableWidgetStack* RootStack;
};
