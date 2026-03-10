// Vince Petrelli All Rights Reserved


#include "Widgets/InGameWidgetLayout.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UCommonActivatableWidget* UInGameWidgetLayout::PushWidgetToStack(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
    if (RootStack && WidgetClass)
    {
        // 스택에 위젯을 추가하고 활성화된 인스턴스를 반환
        return RootStack->AddWidget(WidgetClass);
    }
    return nullptr;
}
