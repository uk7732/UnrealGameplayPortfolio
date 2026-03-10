// Vince Petrelli All Rights Reserved


#include "Widgets/Item/PotionWidgetBase.h"
#include "Widgets/Item/ItemSlotWidgetBase.h"
#include "Characters/CC_PlayerCharacter.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "Components/TextBlock.h"

void UPotionWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 플레이어 스테이트 및 인벤토리 컴포넌트 찾기
	APawn* OwningPawn = GetOwningPlayerPawn();
	ACC_PlayerCharacter* PlayerChar = Cast<ACC_PlayerCharacter>(OwningPawn);

	if (PlayerChar)
	{
		ACC_PlayerState* PS = PlayerChar->GetPlayerState<ACC_PlayerState>();
		if (PS)
		{
			UInventoryComponent* InvComp = PS->GetInventory();
			if (InvComp)
			{
				// 위젯이 보이도록 설정
				SetVisibility(ESlateVisibility::HitTestInvisible);

				// 2. 인벤토리 변경 델리게이트 바인딩
				// 인벤토리에 아이템이 들어오거나 나갈 때마다 UI를 갱신합니다.
				InvComp->OnInventoryUpdated.RemoveDynamic(this, &UPotionWidgetBase::UpdatePotionUI);
				InvComp->OnInventoryUpdated.AddDynamic(this, &UPotionWidgetBase::UpdatePotionUI);

				// 3. 초기값 갱신
				UpdatePotionUI();
				return;
			}
		}
	}

	// 캐릭터나 인벤토리를 못 찾으면 숨김
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPotionWidgetBase::UpdatePotionUI()
{
	if (!PotionSlot) return;

	// 1. 인벤토리 컴포넌트 다시 가져오기
	APawn* OwningPawn = GetOwningPlayerPawn();
	ACC_PlayerCharacter* PlayerChar = Cast<ACC_PlayerCharacter>(OwningPawn);
	if (!PlayerChar) return;

	ACC_PlayerState* PS = PlayerChar->GetPlayerState<ACC_PlayerState>();
	if (!PS) return;

	UInventoryComponent* InvComp = PS->GetInventory();
	if (!InvComp) return;

	// 2. 현재 인벤토리에 있는 해당 포션의 총 갯수 가져오기
	int32 CurrentCount = InvComp->GetItemCount(PotionItemID);

	// 3. 슬롯에 표시할 가짜 데이터 생성
	FItemData DisplayData;
	DisplayData.TID = PotionItemID; // 테이블 조회를 위해 ID 설정
	DisplayData.Count = CurrentCount;
	DisplayData.UID = -1; // 단순 표시용이므로 UID 무관

	// 4. 슬롯 UI 갱신
	// ItemSlotWidgetBase가 알아서 TID로 이미지를 찾고, Count를 텍스트로 찍어줍니다.
	PotionSlot->InitSlot(DisplayData);

	// 5. 슬롯 타입 설정 (장비창 타입으로 설정하여 드래그 등 불필요한 상호작용 방지)
	PotionSlot->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
}