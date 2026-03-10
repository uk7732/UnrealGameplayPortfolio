// Vince Petrelli All Rights Reserved


#include "Widgets/Options/Widget_JoinGame.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Components/EditableText.h"
#include "Instance/Admin4_GameInstance.h"

void UWidget_JoinGame::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Connect)
	{
		Btn_Connect->OnClicked().AddUObject(this, &ThisClass::OnConnectClicked);
	}
	if (Btn_Cancel)
	{
		Btn_Cancel->OnClicked().AddUObject(this, &ThisClass::OnCancelClicked);
	}
}

UWidget* UWidget_JoinGame::NativeGetDesiredFocusTarget() const
{
	// 팝업이 뜨면 바로 IP 입력칸에 포커스를 줍니다.
	if (Edit_IPAddress)
	{
		return Edit_IPAddress;
	}

	// 만약 입력칸이 없으면 버튼이라도 잡게 함
	if (Btn_Connect)
	{
		return Btn_Connect;
	}

	return Super::NativeGetDesiredFocusTarget();
}

void UWidget_JoinGame::OnConnectClicked()
{
	if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetGameInstance()))
	{
		FString TargetIP = TEXT("127.0.0.1");

		if (Edit_IPAddress)
		{
			FString InputText = Edit_IPAddress->GetText().ToString();
			if (!InputText.IsEmpty())
			{
				TargetIP = InputText;
			}
		}

		// 접속 시도
		GI->JoinMultiplayer(TargetIP);
	}
}

void UWidget_JoinGame::OnCancelClicked()
{
	// 위젯 스택에서 제거 (팝업 닫기)
	DeactivateWidget();
}