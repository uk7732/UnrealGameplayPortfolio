// Vince Petrelli All Rights Reserved


#include "Widgets/WidgetNPC/Widget_MapList.h"
#include "Characters/CC_NPCCharacter.h"
#include "Widgets/WidgetNPC/Widget_MapEntry.h"
#include "Widgets/WidgetNPC/Widget_ConfirmPopup.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"

void UWidget_MapList::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Prev) Btn_Prev->OnClicked.AddDynamic(this, &UWidget_MapList::OnClick_PrevPage);
	if (Btn_Next) Btn_Next->OnClicked.AddDynamic(this, &UWidget_MapList::OnClick_NextPage);
	if (Btn_Transfer) Btn_Transfer->OnClicked.AddDynamic(this, &UWidget_MapList::OnClick_Transfer);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &UWidget_MapList::OnClick_Exit);
}

void UWidget_MapList::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	// 1. 내부 데이터 초기화
	SelectedMapTID = FName();
	SelectedMapName = FText::GetEmpty();
	CurrentPageIndex = 0;
	AllMaps.Empty();

	// 2. 우측 상세 정보 UI 초기화
	if (Text_LargeMapName) Text_LargeMapName->SetText(FText::GetEmpty());
	if (Image_LargePreview) Image_LargePreview->SetBrushFromTexture(nullptr);
	if (Btn_Transfer) Btn_Transfer->SetIsEnabled(false);

	// 3. 자식 위젯 메모리 해제
	if (VB_MapList) VB_MapList->ClearChildren();
}

void UWidget_MapList::Init(ACC_NPCCharacter* NPC)
{
	Super::Init(NPC);

	if (!OwnerNPC.IsValid())
		return;

	AllMaps = OwnerNPC->GetMapList();


	CurrentPageIndex = 0;
	MapListRefresh();
}

void UWidget_MapList::MapListRefresh()
{
	if (!OwnerNPC.IsValid())
		return;
	
	if (!VB_MapList || !EntryWidgetClass)
		return;

	VB_MapList->ClearChildren();

	if (Btn_Transfer)
	{
		Btn_Transfer->SetIsEnabled(false);
	}
	SelectedMapTID = FName();

	int32 TotalCount = AllMaps.Num();
	int32 StartIndex = CurrentPageIndex * MaxWidgetsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + MaxWidgetsPerPage, TotalCount);

	for (int32 i = StartIndex; i < EndIndex; ++i)
	{
		UWidget_MapEntry* NewEntry = CreateWidget<UWidget_MapEntry>(this, EntryWidgetClass);
		if (NewEntry)
		{
			// 데이터 주입 (Setup 함수)
			NewEntry->Setup(AllMaps[i]);

			NewEntry->OnEntryClicked.AddDynamic(this, &UWidget_MapList::OnClick_Entry);
			// 박스에 추가
			VB_MapList->AddChild(NewEntry);
		}
	}
	// 4. 총 페이지 수 계산 (올림 처리)
	// 예: 11개 / 5 = 2.2 -> 3페이지
	int32 TotalPages = FMath::Max(FMath::CeilToInt((float)TotalCount / (float)MaxWidgetsPerPage), 1);
	// 데이터가 0개여도 최소 1페이지는 보여주려면 Max(1, TotalPages) 처리 가능

	// 5. 버튼 활성화/ 비활성화 처리
	if (Btn_Prev)
	{
		Btn_Prev->SetIsEnabled(CurrentPageIndex > 0);
	}
	
	if (Btn_Next)
	{
		Btn_Next->SetIsEnabled(CurrentPageIndex < TotalPages - 1);
	}

	if (Text_PageInfo)
	{
		FString PageStr = FString::Printf(TEXT("%d / %d"), CurrentPageIndex + 1, TotalPages);
		Text_PageInfo->SetText(FText::FromString(PageStr));
	}
}

void UWidget_MapList::OnClick_NextPage()
{
	int TotalCount = AllMaps.Num();
	int32 TotalPages = FMath::CeilToInt((float)TotalCount / MaxWidgetsPerPage);

	if (CurrentPageIndex < TotalPages - 1)
	{
		CurrentPageIndex++;
		MapListRefresh();
	}
}

void UWidget_MapList::OnClick_PrevPage()
{
	if (CurrentPageIndex > 0)
	{
		CurrentPageIndex--;
		MapListRefresh();
	}

}

void UWidget_MapList::OnClick_Transfer()
{
	if (SelectedMapTID.IsNone())
		return;

	if (PopupWidgetClass)
	{
		UWidget_ConfirmPopup* Popup = CreateWidget<UWidget_ConfirmPopup>(this, PopupWidgetClass);
		if (Popup)
		{
			Popup->SetupPopup(SelectedMapTID, SelectedMapName);
			Popup->AddToViewport(999);
			Popup->SetIsFocusable(true);
			Popup->SetFocus();
		}
	}
}

void UWidget_MapList::OnClick_Exit()
{
	DeactivateWidget();
}

void UWidget_MapList::OnImageLoaded(TSoftObjectPtr<UTexture2D> LoadedTexture, FName RequestedMapTID)
{
	// 핵심 로직: 방금 로딩이 끝난 이미지가, '현재 선택된 맵'의 이미지가 맞는지 검증
	if (SelectedMapTID != RequestedMapTID)
	{
		// 사용자가 로딩 중에 다른 맵을 클릭했으므로, 이 이미지는 무시하고 버립니다.
		return;
	}

	// 유효성 검사 후 최종 UI 적용
	if (Image_LargePreview && LoadedTexture.IsValid())
	{
		Image_LargePreview->SetBrushFromTexture(LoadedTexture.Get());
	}
}

void UWidget_MapList::OnClick_Entry(const FMapData& Data)
{
	SelectedMapTID = Data.MapTID;
	SelectedMapName = Data.DisplayName;


	if (Text_LargeMapName)
	{
		Text_LargeMapName->SetText(Data.DisplayName);
	}

	if (Btn_Transfer)
	{
		Btn_Transfer->SetIsEnabled(true);
	}

	TSoftObjectPtr<UTexture2D> TexturePtr = Data.MapImage;

	// 1. 이미 로드되어 있는 경우 (동기 방식처럼 즉시 처리)
	if (TexturePtr.IsValid())
	{
		OnImageLoaded(TexturePtr, Data.MapTID);
	}
	else if (TexturePtr.IsPending())
	{
		// AssetManager가 정상적으로 존재하는지 안전하게 확인 (C++의 방어적 프로그래밍)
		if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
		{
			FStreamableManager& Streamable = AssetManager->GetStreamableManager();

			// C++ 문법: CreateUObject의 'Payload' (추가 매개변수 전달)
			// 함수 포인터 뒤에 매개변수들을 적어주면, 로딩 완료 시 OnImageLoaded 함수에 이 값들이 그대로 전달됩니다.
			FStreamableDelegate LoadDelegate = FStreamableDelegate::CreateUObject(
				this,
				&UWidget_MapList::OnImageLoaded,
				TexturePtr,
				Data.MapTID // 요청 당시의 MapTID를 영수증처럼 같이 보냄
			);

			Streamable.RequestAsyncLoad(TexturePtr.ToSoftObjectPath(), LoadDelegate);
		}
	}
}
