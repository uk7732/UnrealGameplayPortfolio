// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Common/GameDatas.h"
#include "CommonButtonBase.h"
#include "Widget_MapEntry.generated.h"

class UCommonButtonBase;
class UButton;
class UTextBlock;
class UImage;
class UWidget_ConfirmPopup;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapEntryClicked, const FMapData&, MapData);

UCLASS()
class UWidget_MapEntry : public UCommonButtonBase
{

	GENERATED_BODY()

public:
	// 부모(List)가 데이터를 넣어줄 때 호출하는 함수
	void Setup(const FMapData& InData);

	// 이 델리게이트를 통해 부모가 구독(Bind)할 수 있게 합니다.
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMapEntryClicked OnEntryClicked;

protected:
	// [핵심] 부모님(CommonButtonBase)이 물려준 "클릭 함수"를 내 입맛대로 고쳐 씁니다.
	virtual void NativeOnClicked() override;

private:
	FMapData CachedMapData;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_MapName;

	UPROPERTY()
	FText MyMapName;

	FName MyMapTID;
};
