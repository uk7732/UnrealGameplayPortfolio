// Vince Petrelli All Rights Reserved


#include "Widgets/Widget_InGameActivatableWidget.h"
#include "Characters/CC_NPCCharacter.h"
#include "Components/Button.h"




void UWidget_InGameActivatableWidget::UpdateUI()
{

}

void UWidget_InGameActivatableWidget::Init(ACC_NPCCharacter* NPC)
{
	OwnerNPC = NPC;	
}

TOptional<FUIInputConfig> UWidget_InGameActivatableWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}


void UWidget_InGameActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UWidget_InGameActivatableWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

