// Vince Petrelli All Rights Reserved


#include "Widgets/Item/InventoryWindowWidget.h"
#include "Widgets/Item/InventoryWidgetBase.h"
#include "Widgets/Item/EquipmentWidgetBase.h"
#include "Widgets/Item/ItemDragDropOperation.h" 
#include "Component/InventoryComponent.h"
#include "GameMode/CC_PlayerState.h"           


void UInventoryWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerState를 가져와서 하위 위젯들에게 컴포넌트를 연결해줍니다.
	if (APlayerState* PS = GetOwningPlayerState())
	{
		if (ACC_PlayerState* FrontendPS = Cast<ACC_PlayerState>(PS))
		{
			// 1. 파란색 영역 연결
			if (WBP_InventoryGrid)
			{
				// InventoryWidgetBase.cpp의 NativeConstruct 로직을 함수(BindInventory)로 분리하는 게 좋습니다.
				// 현재는 NativeConstruct에서 자동 처리되므로 놔둬도 되지만, 명시적 호출이 더 안전합니다.
				// WBP_InventoryGrid->RefreshUI(); // 필요 시 호출
			}

			// 2. 노란색 영역 연결
			if (WBP_Equipment)
			{
				WBP_Equipment->BindEquipment(FrontendPS->GetEquipment());
			}
		}
	}
}

bool UInventoryWindowWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 1. 드롭된 정보가 우리가 만든 아이템 드래그 정보인지 확인합니다.
	UItemDragDropOperation* DragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!DragOp)
	{
		// 아이템이 아니면 기본 로직을 따릅니다.
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}	

	// 2. 인벤토리 컴포넌트 찾기
	ACC_PlayerState* PS = Cast<ACC_PlayerState>(GetOwningPlayerState());
	if (!PS) return false;

	UInventoryComponent* InvComp = PS->GetInventory();
	if (!InvComp) return false;

	// 3. 아이템 버리기 요청
	// (슬롯 위젯이 아닌, 윈도우 배경에 떨어뜨렸으므로 '버리기'로 간주)
	InvComp->RequestDropItem(DragOp->ItemTID, DragOp->ItemUID);

	// 로그 확인용
	UE_LOG(LogTemp, Warning, TEXT("아이템 버리기 요청: %s"), *DragOp->ItemTID.ToString());

	return true; // 드롭 처리 완료
}
