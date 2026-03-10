// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_InGameActivatableWidget.h"
#include "Common/GameDatas.h"
#include "Widget_MapList.generated.h"


class UVerticalBox; // 또는 UScrollBox
class UButton;
class UTextBlock;
class ACC_NPCCharacter;
class UImage;
class UWidget_ConfirmPopup;
/**
 * 
 */
UCLASS()
class FRONTENDUI_API UWidget_MapList : public UWidget_InGameActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDeactivated() override;
	virtual void Init(ACC_NPCCharacter* NPC) override;

	void MapListRefresh();

	UFUNCTION() 
	void OnClick_NextPage();
	UFUNCTION()
	void OnClick_PrevPage();
	UFUNCTION()
	void OnClick_Transfer();

	UFUNCTION()
	void OnClick_Exit();

	UFUNCTION()
	void OnImageLoaded(TSoftObjectPtr<UTexture2D> LoadedTexture, FName RequestedMapTID);

	UFUNCTION()
	void OnClick_Entry(const FMapData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UWidget_ConfirmPopup> PopupWidgetClass;

private:
	FName SelectedMapTID;
	FText SelectedMapName;

	// 좌측 상세 창 UI 컴포넌트들 (블루프린트에서 이름 맞추기)
	UPROPERTY(meta = (BindWidget))
	UImage* Image_LargePreview;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_LargeMapName;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Transfer; // 최종 "이동하기" 버튼

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	// --- UI 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_MapList; // 여기에 Entry들이 쌓임

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Prev; // << 버튼

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Next; // >> 버튼


	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PageInfo; // "1 / 3" 표시용

	// --- 데이터 ---
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWidget_MapEntry> EntryWidgetClass; // BP에서 설정할 자식 위젯 클래스

	TArray<FMapData> AllMaps; // 전체 맵 리스트 복사본

	// --- 페이징 변수 ---
	int32 CurrentPageIndex = 0;
	int32 MaxWidgetsPerPage = 5;
};
