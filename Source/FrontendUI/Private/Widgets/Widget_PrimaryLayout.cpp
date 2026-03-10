// Vince Petrelli All Rights Reserved


#include "Widgets/Widget_PrimaryLayout.h"
#include "Subsytems/FrontendUISubsystem.h" 
#include "FrontendDebugHelper.h"

void UWidget_PrimaryLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 1. UI 서브시스템 가져오기
	if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(GetOwningLocalPlayer()))
	{
		// 2. 나 자신(PrimaryLayout)을 서브시스템에 등록
		UISubsystem->RegisterCreatedPrimaryLayoutWidget(this);
	}
}

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{	
	// 태크만으로 특정 레이어(스택 : Modal, GameMenu 등)를 즉시 찾아냄(O(1) Map 조회) 시간 복잡도 활용
	checkf(RegisteredWidgetStackMap.Contains(InTag),TEXT("Can not find the widget stack by the tag %s"),*InTag.ToString());

	return RegisteredWidgetStackMap.FindRef(InTag);
}

// GameplayTag를 Key로 사용하여 물리적인 위젯 스택(Container) 관리
void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (!IsDesignTime())
	{
		// 런타임에 동적으로 태그와 위젯 컨테이너 매핑
		if (!RegisteredWidgetStackMap.Contains(InStackTag))
		{
			RegisteredWidgetStackMap.Add(InStackTag,InStack);
		}
	}
}
