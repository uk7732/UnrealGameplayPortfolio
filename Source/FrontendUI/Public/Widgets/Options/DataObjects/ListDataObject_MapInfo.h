// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "Table/TableDatas.h" 
#include "ListDataObject_MapInfo.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UListDataObject_MapInfo : public UListDataObject_Base
{
	GENERATED_BODY()

private:
    FString MapPath;
    FString DistanceInfo;
    FString ElevationInfo;

public:
    // 테이블 행 데이터를 받아서 멤버 변수에 세팅하는 함수
    void InitFromTableRow(FName InRowName, const FMapTableRow& Row)
    {
        SetDataID(InRowName); // RowName을 ID로 사용
        SetDataDisplayName(Row.DisplayName);
        SetDescriptionRichText(Row.Description);
        SetSoftDescriptionImage(Row.MapImage);

        MapPath = Row.MapPath;        
    }

    // Getter
    FString GetMapPath() const { return MapPath; }
    FString GetDistanceInfo() const { return DistanceInfo; }
    FString GetElevationInfo() const { return ElevationInfo; }

};