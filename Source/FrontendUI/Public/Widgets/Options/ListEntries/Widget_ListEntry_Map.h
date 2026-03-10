// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widgets/Options/DataObjects/ListDataObject_MapInfo.h"
#include "CommonTextBlock.h" 

#include "Widget_ListEntry_Map.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UWidget_ListEntry_Map : public UWidget_ListEntry_Base
{
	GENERATED_BODY()

private:
    UPROPERTY(meta = (BindWidgetOptional))
    UCommonTextBlock* Txt_MapName;

protected:
    virtual void OnOwningListDataObjectSet(UListDataObject_Base* InDataObject) override
    {
        Super::OnOwningListDataObjectSet(InDataObject);

        if (auto* MapData = Cast<UListDataObject_MapInfo>(InDataObject))
        {
            // 부모 클래스에 있는 CommonText_SettingDisplayName이 있다면 자동 세팅되지만,
            // 커스텀 디자인을 위해 별도 바인딩
            if (Txt_MapName) Txt_MapName->SetText(MapData->GetDataDisplayName());      
        }
    }    
};
