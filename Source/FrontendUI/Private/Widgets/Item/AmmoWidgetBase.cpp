// Vince Petrelli All Rights Reserved


#include "Widgets/Item/AmmoWidgetBase.h"
#include "Widgets/Item/ItemSlotWidgetBase.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Components/TextBlock.h"

void UAmmoWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 플레이어 캐릭터 찾기
	APawn* OwningPawn = GetOwningPlayerPawn();
	ACC_PlayerCharacter* PlayerChar = Cast<ACC_PlayerCharacter>(OwningPawn);

	if (PlayerChar)
	{
		// 캐릭터가 있다면 -> 보인다 (HitTestInvisible: 보이지만 클릭은 통과)
		SetVisibility(ESlateVisibility::HitTestInvisible);

		// 2. 델리게이트 바인딩 (탄약 수 변경 시 호출)
		// 기존 바인딩 제거 후 추가 (안전장치)
		PlayerChar->OnAmmoChanged.RemoveDynamic(this, &UAmmoWidgetBase::UpdateAmmoUI);
		PlayerChar->OnAmmoChanged.AddDynamic(this, &UAmmoWidgetBase::UpdateAmmoUI);

		// 3. 초기값 설정 (현재 상태로 즉시 갱신)
		UpdateAmmoUI(PlayerChar->GetCurrentAmmoCount(), PlayerChar->GetMaxAmmoCount());
	}
	else
	{
		// 캐릭터가 없다면 (메인 메뉴 등) -> 숨긴다 (Collapsed: 공간도 차지하지 않음)
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UAmmoWidgetBase::UpdateAmmoUI(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoSlot) return;

	// 4. 표시용 가짜 데이터 생성
	// 기존 UItemSlotWidgetBase::InitSlot 로직을 재사용하기 위해 FItemData 구조체를 만듭니다.
	FItemData DisplayData;
	DisplayData.TID = AmmoItemID; // "Ammo" -> 테이블에서 아이콘을 가져오기 위함
	DisplayData.Count = CurrentAmmo; // 현재 장전된 탄약 수
	DisplayData.UID = -1; // 단순 표시용이므로 UID는 필요 없음

	// 5. 슬롯 UI 갱신
	// 기존 슬롯 위젯이 알아서 테이블을 조회해 이미지를 세팅하고, Count를 텍스트로 띄워줍니다.
	AmmoSlot->InitSlot(DisplayData);

	// 6. (선택사항) 슬롯 타입 설정 (드래그 앤 드롭 방지용)
	// Equipment 타입으로 설정하여 인벤토리로의 드래그 등 불필요한 상호작용을 막을 수 있습니다.
	// 혹은 UItemSlotWidgetBase에 Interaction을 끄는 옵션을 추가해도 됩니다.
	AmmoSlot->SetSlotType(UItemSlotWidgetBase::ESlotType::Equipment);
}