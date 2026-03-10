// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_MapSelectionScreen.generated.h"

class UFrontendCommonListView;
class UCommonTextBlock;
class UCommonLazyImage;
class UFrontendCommonButtonBase;
class UListDataObject_MapInfo;

UCLASS()
class FRONTENDUI_API UWidget_MapSelectionScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

private:
	// 데이터 테이블 로드 및 리스트 생성
	void InitMapListFromTable();

	// 리스트 아이템 선택 시
	void OnMapSelected(UObject* Item);

	// 확인(이동) 버튼 클릭 시
	void OnConfirmClicked();

	// 취소(닫기) 버튼 클릭 시
	void OnCancelClicked();

	UPROPERTY(meta = (BindWidget))
	UFrontendCommonListView* List_MapRoutes; // 좌측 리스트

	// 우측 상세 패널
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Txt_DetailTitle;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Txt_DetailDesc;
	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* Img_DetailMap;

	UPROPERTY(meta = (BindWidget))
	UFrontendCommonButtonBase* Btn_Confirm;
	UPROPERTY(meta = (BindWidget))
	UFrontendCommonButtonBase* Btn_Cancel;

	UPROPERTY(Transient)
	UListDataObject_MapInfo* CurrentSelectedMapData;
};
