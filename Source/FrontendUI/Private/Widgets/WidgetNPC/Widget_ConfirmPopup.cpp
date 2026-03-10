// Vince Petrelli All Rights Reserved


#include "Widgets/WidgetNPC/Widget_ConfirmPopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Controllers/CC_PlayerController.h"

void UWidget_ConfirmPopup::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Yes) Btn_Yes->OnClicked.AddDynamic(this, &UWidget_ConfirmPopup::OnClick_Yes);
	if (Btn_No)  Btn_No->OnClicked.AddDynamic(this, &UWidget_ConfirmPopup::OnClick_No);
}

void UWidget_ConfirmPopup::SetupPopup(FName InTargetMapTID, FText InMapName)
{
	TargetMapTID = InTargetMapTID;

	if (Text_Message)
	{
		FText FormattedMessage = FText::Format(FText::FromString(TEXT("[{0}] 맵으로 이동하시겠습니까?")),
			InMapName
		);

		Text_Message->SetText(FormattedMessage);
	}
}

void UWidget_ConfirmPopup::OnClick_Yes()
{
	if (ACC_PlayerController* PC = GetOwningPlayer<ACC_PlayerController>())
	{
		PC->Server_ExecuteLevelTransfer(TargetMapTID);
	}

	//화면에서 이 팝업을 즉시 지움
	RemoveFromParent();
}

void UWidget_ConfirmPopup::OnClick_No()
{
	// 취소했으므로 팝업창만 화면에서 분리(제거)합니다.
	RemoveFromParent();
}
