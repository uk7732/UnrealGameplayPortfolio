// Vince Petrelli All Rights Reserved


#include "Widgets/WidgetNPC/Widget_MapEntry.h"
#include "Widgets/WidgetNPC/Widget_ConfirmPopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Controllers/CC_PlayerController.h"



void UWidget_MapEntry::Setup(const FMapData& InData)
{
	MyMapTID = InData.MapTID;

	MyMapName = InData.DisplayName;

	CachedMapData = InData;

	if (Text_MapName)
	{
		Text_MapName->SetText(MyMapName);
	}
}

void UWidget_MapEntry::NativeOnClicked()
{
	Super::NativeOnClicked();

	
	OnEntryClicked.Broadcast(CachedMapData);
}
