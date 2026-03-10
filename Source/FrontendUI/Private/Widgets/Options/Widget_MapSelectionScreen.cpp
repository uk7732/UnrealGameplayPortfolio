// Vince Petrelli All Rights Reserved


#include "Widgets/Options/Widget_MapSelectionScreen.h"
#include "Widgets/Options/DataObjects/ListDataObject_MapInfo.h"
#include "Instance/TableSubsystem.h"
#include "Widgets/Components/FrontendCommonListView.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Instance/Admin4_GameInstance.h"
#include "CommonTextBlock.h"
#include "CommonLazyImage.h"


void UWidget_MapSelectionScreen::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (List_MapRoutes)
    {
        List_MapRoutes->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnMapSelected);
    }
    if (Btn_Confirm)
    {
        Btn_Confirm->OnClicked().AddUObject(this, &ThisClass::OnConfirmClicked);
    }
    if (Btn_Cancel)
    {
        Btn_Cancel->OnClicked().AddUObject(this, &ThisClass::OnCancelClicked);
    }
}

void UWidget_MapSelectionScreen::NativeOnActivated()
{
    Super::NativeOnActivated();
    InitMapListFromTable();
}

void UWidget_MapSelectionScreen::InitMapListFromTable()
{
    // 1. 테이블 서브시스템 가져오기
    UTableSubsystem* TableSys = UTableSubsystem::Get(GetOwningLocalPlayer());
    if (!TableSys) return;

    // 2. MapTable 가져오기 (TABLE_NAME::MAP 사용)
    const UDataTable* MapTable = TableSys->GetDataTable(TABLE_NAME::MAP);
    if (!MapTable) return;

    TArray<UListDataObject_MapInfo*> ListItems;

    // 3. 테이블의 모든 행 순회
    FString ContextStr;
    for (auto& RowName : MapTable->GetRowNames())
    {
        FMapTableRow* Row = MapTable->FindRow<FMapTableRow>(RowName, ContextStr);
        if (Row)
        {
            // 데이터 객체 생성 및 초기화
            UListDataObject_MapInfo* NewData = NewObject<UListDataObject_MapInfo>(this);
            NewData->InitFromTableRow(RowName, *Row);
            ListItems.Add(NewData);
        }
    }

    // 4. 리스트뷰에 세팅
    if (List_MapRoutes)
    {
        List_MapRoutes->SetListItems(ListItems);

        // 첫 번째 항목 자동 선택
        if (ListItems.Num() > 0)
        {
            List_MapRoutes->SetSelectedItem(ListItems[0]);
        }
    }
}

void UWidget_MapSelectionScreen::OnMapSelected(UObject* Item)
{
    CurrentSelectedMapData = Cast<UListDataObject_MapInfo>(Item);
    if (!CurrentSelectedMapData) return;

    // 우측 UI 갱신
    if (Txt_DetailTitle) Txt_DetailTitle->SetText(CurrentSelectedMapData->GetDataDisplayName());
    if (Txt_DetailDesc) Txt_DetailDesc->SetText(CurrentSelectedMapData->GetDescriptionRichText());

    if (Img_DetailMap)
    {
        Img_DetailMap->SetBrushFromLazyTexture(CurrentSelectedMapData->GetSoftDescriptionImage());
    }
}

void UWidget_MapSelectionScreen::OnConfirmClicked()
{
    if (!CurrentSelectedMapData) return;

    // GameInstance를 통해 실제 맵 이동 요청
    if (UAdmin4_GameInstance* GI = Cast<UAdmin4_GameInstance>(GetGameInstance()))
    {
        // FName(String) 변환하여 전달
        /*FName LevelName = FName(*CurrentSelectedMapData->GetMapPath());*/
        FString FullPath = CurrentSelectedMapData->GetMapPath();
        FString LevelName = FPackageName::GetShortName(FullPath);

        // 싱글/멀티 여부에 따라 적절한 함수 호출
        // 예: GI->LaunchSingleplayer(LevelName); 
        // 혹은 직접 ServerTravel 로직 수행

        // (주의) MapPath가 전체 경로(/Game/Maps/Lobby)라면 패키징 빌드에서는 
        // 맵 이름만 필요할 수도 있으므로 FPackageName::GetShortName 등을 활용하세요.        
        GI->NextMap(FullPath);
    }
}

void UWidget_MapSelectionScreen::OnCancelClicked()
{
    DeactivateWidget();
}