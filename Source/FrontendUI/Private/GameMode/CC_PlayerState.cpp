// Vince Petrelli All Rights Reserved


#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "Component/EquipmentComponent.h"

//ESlateVisibility enum 값
//Visible - 표시됨 정상적으로 렌더링 되며 늨릭 및 상호작용이 가능														 -> UI의 기본 상태
//Hidden - 숨겨짐 렌더링되지는 않지만 원래 크기만큼 공간을 차지															 -> 레이아웃이 변경되지 않아야 할 때
//Collapsed - 축소됨 렌더링되지 않으며 레이아웃에서 공간을 전혀 차지하지 않음											 -> 위젯을 완전히 제거하여 레이아웃을 밀어 올릴 때
//HitTestInvisible - 클릭통과 히트 테스트 불가 렌더링되지만 클릭 등 상호작용이 불가능									 -> 배경이나 장식용 위젯
//SelfHitTestInvisible - 자체 클릭통과 히트 테스트 불가 자기 자신은 상호작용이 불가능하지만 자식 위젯들은 상호작용 가능  -> 컨테이너 위젯에 사용

ACC_PlayerState::ACC_PlayerState()
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	Equipment = CreateDefaultSubobject<UEquipmentComponent>("Equipment");
}

UInventoryComponent* ACC_PlayerState::GetInventory()
{
	return Inventory;
}

UEquipmentComponent* ACC_PlayerState::GetEquipment()
{
	return Equipment;
}

void ACC_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ACC_PlayerState, Inventory);
	DOREPLIFETIME(ACC_PlayerState, Equipment);
}
