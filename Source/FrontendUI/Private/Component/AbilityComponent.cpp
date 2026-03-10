// Vince Petrelli All Rights Reserved


#include "Component/AbilityComponent.h"
#include "Instance/Tablesubsystem.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}



// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MPRegenTimer,
			this,
			&UAbilityComponent::server_RegenMP,
			0.1f,   // 0.1초마다 계산
			true
		);
		GetWorld()->GetTimerManager().SetTimer(
			BuffTimerHandle,
			this,
			&UAbilityComponent::server_UpdateBuffs_DeBuffs,
			0.1f,
			true
		);
	}

}

void UAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(MPRegenTimer);
	GetWorld()->GetTimerManager().ClearTimer(BuffTimerHandle);
}

void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

}

//여러개입력
void UAbilityComponent::AddAbilData(eAbilCategoryType category, const TArray<FAbilityData>& abil)
{
	switch (category)
	{
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP:
		AddEquipAbil(abil);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_OWNED:
		AddOwnedAbil(abil);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_BUFF:
		AddBuffAbil(abil);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_DEBUFF:
		AddDeBuffAbil(abil);
	default:
		break;
	}

	RefreshTotalAbil();
}


//단일입력
void UAbilityComponent::AddAbilData(eAbilCategoryType category, const FAbilityData& abil)
{
	switch (category)
	{
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP:
		AddEquipAbil(abil);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_OWNED:
		AddOwnedAbil(abil);
		break;	
	case eAbilCategoryType::ABIL_CATEGORY_BUFF:
		AddBuffAbil(abil);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_DEBUFF:
		AddDeBuffAbil(abil);
	default:
		break;
	}

	RefreshTotalAbil();
}

void UAbilityComponent::EraseAbilData(eAbilCategoryType category, FName TID)
{
	switch (category)
	{
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_EQUIP:
		EraseEquipAbil(TID);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_ITEM_OWNED:
		EraseOwnedAbil(TID);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_BUFF:
		EraseBuffAbil(TID);
		break;
	case eAbilCategoryType::ABIL_CATEGORY_DEBUFF:
		EraseDeBuffAbil(TID);
	default:
		break;
	}
	RefreshTotalAbil();
}

const FAbilityData* UAbilityComponent::GetAbil(eAbilCategoryType category, eAbilType type)
{
	switch (category)
	{
	case eAbilCategoryType::ABIL_CATEGORY_DEFAULT: return GetDefaultAbil(type);
	case eAbilCategoryType::ABIL_CATEGORY_TOTAL: return GetTotalAbil(type);
	default:
		break;
	}

	return nullptr;
}

void UAbilityComponent::SetDefaultAbil(const TArray<FAbilityData>& abil)
{
	server_SetDefaultAbil(abil);
}

void UAbilityComponent::ApplyDefaultAbil(const FAbilityData& abil)
{
	server_ApplydefaultAbil(abil);
}

const FAbilityData* UAbilityComponent::GetDefaultAbil(eAbilType type)
{
	return DefaultAbils.FindByKey(type);
}

const FAbilityData* UAbilityComponent::GetTotalAbil(eAbilType type)
{
	return TotalAbils.FindByKey(type);
}

void UAbilityComponent::AddEquipAbil(const TArray<FAbilityData>& abil)
{
	for (auto& it : abil)
	{
		ItemEquipAbils.Add(it);
	}
}


void UAbilityComponent::AddEquipAbil(const FAbilityData& abil)
{
	ItemEquipAbils.Add(abil);
}

void UAbilityComponent::AddOwnedAbil(const TArray<FAbilityData>& abil)
{
	for (auto& it : abil)
	{
		OwnedAbils.Add(it);
	}
}

void UAbilityComponent::AddOwnedAbil(const FAbilityData& abil)
{
	OwnedAbils.Add(abil);
}

void UAbilityComponent::AddBuffAbil(const TArray<FAbilityData>& abil)
{
	for (auto& it : abil)
	{
		BuffAbils.Add(it);
	}
}

void UAbilityComponent::AddBuffAbil(const FAbilityData& abil)
{
	BuffAbils.Add(abil);
}


void UAbilityComponent::EraseEquipAbil(FName TID)
{
	FName tid = TID;
	ItemEquipAbils.RemoveAll([tid](const FAbilityData& Data)
		{
			return Data.TID == tid;
		});
}

void UAbilityComponent::EraseOwnedAbil(FName TID)
{
	FName tid = TID;
	OwnedAbils.RemoveAll([tid](const FAbilityData& Data)
		{
			return Data.TID == tid;
		});
}

void UAbilityComponent::EraseBuffAbil(FName TID)
{
	FName tid = TID;
	BuffAbils.RemoveAll([tid](const FAbilityData& Data)
		{
			return Data.TID == tid;
		});
}

void UAbilityComponent::AddDeBuffAbil(const TArray<FAbilityData>& abil)
{
	for (auto& it : abil)
	{
		DeBuffAbils.Add(it);
	}
}

void UAbilityComponent::AddDeBuffAbil(const FAbilityData& abil)
{
	DeBuffAbils.Add(abil);
}

void UAbilityComponent::EraseDeBuffAbil(FName TID)
{
	FName tid = TID;
	DeBuffAbils.RemoveAll([tid](const FAbilityData& Data)
		{
			return Data.TID == tid;
		});
}

void UAbilityComponent::RefreshTotalAbil()
{
	//기본 능력치
	//공격력 : 10
	//방어력 : 10
	//체력 : 5
	//마력 : 5

	//아이템 장착 으로 올라가는 능력치
	//공격력 : 3
	//방어력 : 5
	//체력 : 10
	//마력 : 0

	//버프로 올라가는 능력치
	//공격력 : 0
	//방어력 : 5
	//체력 : 0
	//마력 : 0

	//TotalAbils <--전부 계산해서 최종을 들고 있는거
	//공격력 : 13
	//방어력 : 20
	//체력 : 15
	//마력 : 5

	//===HP, MP 갱신을 위한 값====
	float PrevMaxHP = 0.0f;
	float PrevMaxMP = 0.0f;
	float PrevManaPerSecond = 0.0f;
	//TotalAbils는 현재 가장 최신 합산 결과이므로 여기서 이전 Max 값을 얻을 수 있습니다
	const FAbilityData* PrevMaxHPData = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_HP); 
	const FAbilityData* PrevMaxMPData = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_MP); 
	const FAbilityData* PrevManaPerSecondDaTa = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_MP_PS);

	if (PrevMaxHPData)
		PrevMaxHP = PrevMaxHPData->Value;
	if (PrevMaxMPData)
		PrevMaxMP = PrevMaxMPData->Value;
	if (PrevManaPerSecondDaTa)
		PrevManaPerSecond = PrevManaPerSecondDaTa->Value;

	//===========================



	TMap<eAbilType, FAbilityData> Addmerge;  //단순 수치값
	TMap<eAbilType, FAbilityData> Percentmerge; // 퍼센테이지 값
	// 2. DefaultAbils 합산 (기본 능력치)
	for (auto& itVal : DefaultAbils)
	{
		//defaultAbils 에는 Percentmerge가 없고 합쳐질 로직이 없기 때문에 if체크 하지 않음
			Addmerge.Add(itVal.Type, FAbilityData(itVal));
	}

	// 3. ItemEquipAbils 합산
	for (auto& itVal : ItemEquipAbils)
	{
		if (itVal.ModifierType == eAbilModifierType::ABIL_MODIFIER_PERCENT)
		{
			FAbilityData* abil = Percentmerge.Find(itVal.Type);
			if (abil == nullptr)
				Percentmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
		else
		{
			FAbilityData* abil = Addmerge.Find(itVal.Type);
			if (nullptr == abil)
				Addmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
	}

	// 4. ItemOwnedAbils (버프/디버프) 합산
	for (auto& itVal : OwnedAbils)
	{
		if (itVal.ModifierType == eAbilModifierType::ABIL_MODIFIER_PERCENT)
		{
			FAbilityData* abil = Percentmerge.Find(itVal.Type);
			if (abil == nullptr)
				Percentmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
		else
		{
			FAbilityData* abil = Addmerge.Find(itVal.Type);
			if (nullptr == abil)
				Addmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
	}

	for (auto& itVal : BuffAbils)
	{
		if (itVal.ModifierType == eAbilModifierType::ABIL_MODIFIER_PERCENT)
		{
			FAbilityData* abil = Percentmerge.Find(itVal.Type);
			if (abil == nullptr)
				Percentmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
		else
		{
			FAbilityData* abil = Addmerge.Find(itVal.Type);
			if (nullptr == abil)
				Addmerge.Add(itVal.Type, FAbilityData(itVal));
			else
				abil->Value += itVal.Value;
		}
	}


	// 5. TotalAbils 갱신 (결과 저장)
	TotalAbils.Empty();
	for (auto& it : Addmerge)
	{
		float FlatValue = it.Value.Value;
		float PercentValue = 0.0f; // 기존퍼센트값 초기화

		FAbilityData* PercentData = Percentmerge.Find(it.Key);
		if (PercentData)
		{
			PercentValue = PercentData->Value;
		}

		float finalValue = FlatValue * (1.0f + (PercentValue * 0.01f));

		finalValue = (float)FMath::FloorToInt(finalValue);

		TotalAbils.Add(FAbilityData(it.Key, finalValue));
	}
	
	if (DeBuffAbils.Num() != 0)
	{
		TMap<eAbilType, FAbilityData> Minusmerge;  //단순 수치값
		TMap<eAbilType, FAbilityData> DebuffPercentmerge; // 퍼센테이지 값
		for (auto& it : DeBuffAbils)
		{
			if (it.ModifierType == eAbilModifierType::ABIL_MODIFIER_PERCENT)
			{
				FAbilityData* abil = DebuffPercentmerge.Find(it.Type);
				if (abil == nullptr)
					DebuffPercentmerge.Add(it.Type, it);
				else
					abil->Value += it.Value;
			}
			else if (it.ModifierType == eAbilModifierType::ABIL_MODIFIER_MINUS)
			{
				FAbilityData* abil = Minusmerge.Find(it.Type);
				if (abil == nullptr)
					Minusmerge.Add(it.Type, it);
				else
					abil->Value += it.Value;
			}
		}
		
		for (auto& it : TotalAbils)
		{
			const FAbilityData* MinusData = Minusmerge.Find(it.Type);

			if (MinusData)
			{
				it.Value -= MinusData->Value;
				it.Value = FMath::Max(0.0f, it.Value);
			}		
		}
		for (auto& it : TotalAbils)
		{
			const FAbilityData* DebuffPercentData = DebuffPercentmerge.Find(it.Type);
			if (DebuffPercentData)
			{
				it.Value *= (1.0f - (DebuffPercentData->Value * 0.01f));
				it.Value = FMath::Max(0.0f, it.Value);
			}
		}
	}


	UpdateCurrentHP_And_MP(PrevMaxHP, PrevMaxMP, PrevManaPerSecond);
}

float UAbilityComponent::GetMaxHP()
{
	const FAbilityData* Data = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_HP);
	return (Data != nullptr) ? Data->Value : 1.0f; // 0 나누기 방지
}

float UAbilityComponent::GetMaxMP()
{
	const FAbilityData* Data = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_MP);
	return (Data != nullptr) ? Data->Value : 1.0f;
}

float UAbilityComponent::GetHPRatio()
{
	float Max = GetMaxHP();
	return (Max > 0.f) ? (CurrentHP / Max) : 0.f;
}

float UAbilityComponent::GetMPRatio()
{
	float Max = GetMaxMP();
	return (Max > 0.f) ? (CurrentMP / Max) : 0.f;
}

void UAbilityComponent::UpdateCurrentHP_And_MP(float PrevMaxHP, float PrevMaxMP, float PrevManaPerSecond)
{
	const FAbilityData* NewMaxHPData = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_HP); // TotalAbils에서 새 값 조회
	const FAbilityData* NewMaxMPData = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_MP);   // TotalAbils에서 새 값 조회
	const FAbilityData* PrevManaPerSecondData = GetTotalAbil(eAbilType::ABIL_TYPE_MAX_MP_PS);  //TotalAbils에서 새 값 조회
	if (NewMaxHPData)
	{
		float NewMaxHP = NewMaxHPData->Value;

		// A. Max 값이 처음 지정되거나, 0이었을 경우 (게임 시작 등)
		if (PrevMaxHP <= 0.0f)
		{
			CurrentHP = NewMaxHP; // Full HP로 설정
		}
		// B. Max 값이 변경된 경우 (비율 유지)
		else if (NewMaxHP != PrevMaxHP)
		{
			// (CurrentHP / PrevMaxHP) 비율을 NewMaxHP에 곱함
			CurrentHP = (CurrentHP / PrevMaxHP) * NewMaxHP;
		}

		// C. 마지막으로 CurrentHP가 절대 MaxHP를 초과하지 않도록 보장 (치유량 오버플로우 방지)
		CurrentHP = FMath::Min(CurrentHP, NewMaxHP);
	}

	if (NewMaxMPData)
	{
		float NewMaxMP = NewMaxMPData->Value;

		// A. Max 값이 처음 지정되거나, 0이었을 경우 (게임 시작 등)
		if (PrevMaxMP <= 0.0f)
		{
			CurrentMP = NewMaxMP; // Full HP로 설정
		}
		// B. Max 값이 변경된 경우 (비율 유지)
		else if (NewMaxMP != PrevMaxMP)
		{
			// (CurrentHP / PrevMaxHP) 비율을 NewMaxHP에 곱함
			CurrentMP = (CurrentMP / PrevMaxMP) * NewMaxMP;
		}

		// C. 마지막으로 CurrentHP가 절대 MaxHP를 초과하지 않도록 보장 (치유량 오버플로우 방지)
		CurrentMP = FMath::Min(CurrentMP, NewMaxMP);
	}

	if (PrevManaPerSecondData)
	{
			ManaPerSecond = PrevManaPerSecondData->Value; // Full HP로 설정
	}

	// HP 변경
	float MaxHP = GetMaxHP();
	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, MaxHP);
	}
}


void UAbilityComponent::OnRep_DefaultAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();

	//todo : default abils change
	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_DefaultAbils()"));
}

void UAbilityComponent::OnRep_BuffAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();

	//todo : default abils change
	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_BuffAbils()"));
}

void UAbilityComponent::OnRep_DeBuffAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();

	//todo : default abils change
	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_DeBuffAbils()"));
}

void UAbilityComponent::OnRep_ItemEquipAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();

	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_ItemEquipAbils()"));
}

void UAbilityComponent::OnRep_ItemOwnedAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();


	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_ItemOwnedAbils()"));
}

void UAbilityComponent::OnRep_TotalAbils()
{
	auto gameMode = GetWorld()->GetAuthGameMode();


	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("OnRep_TotalAbils()"));

	// 클라이언트 UI 갱신
	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, GetMaxHP());
	}
}

void UAbilityComponent::OnRep_CurrentHP()
{
	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, GetMaxHP());
	}
}

void UAbilityComponent::OnRep_MP()
{
	//todo : UpdateMPUI 만들기
}


void UAbilityComponent::server_UpdateBuffs_DeBuffs_Implementation()
{
	const float Delta = 0.1f; // Timer 주기

	// BuffAbils 감소
	for (int32 i = BuffAbils.Num() - 1; i >= 0; --i)
	{
		FAbilityData& Buff = BuffAbils[i];
		if (Buff.DurationTime > 0.f)
		{
			Buff.DurationTime -= Delta;
			if (Buff.DurationTime <= 0.f)
			{
				BuffAbils.RemoveAt(i);
			}
		}
	}

	// DeBuffAbils 감소
	for (int32 i = DeBuffAbils.Num() - 1; i >= 0; --i)
	{
		FAbilityData& Debuff = DeBuffAbils[i];
		if (Debuff.DurationTime > 0.f)
		{
			Debuff.DurationTime -= Delta;
			if (Debuff.DurationTime <= 0.f)
			{
				DeBuffAbils.RemoveAt(i);
			}
		}
	}

	// TotalAbils 갱신
	RefreshTotalAbil();
}

//디폴트 스텟 갱신
void UAbilityComponent::server_SetDefaultAbil_Implementation(const TArray<FAbilityData>& abils)
{
	DefaultAbils.Empty();

	for (auto& it : abils)
	{
		DefaultAbils.Add(FAbilityData(it));
	}

	RefreshTotalAbil();
}

//디폴트 스텟 Value 값조정
void UAbilityComponent::server_ApplydefaultAbil_Implementation(const FAbilityData& abil)
{
	FAbilityData* FoundAbil = DefaultAbils.FindByKey(abil.Type);
	if (FoundAbil)
	{
		// 한 줄로 처리: (기존 값 + 변경 값)을 계산한 후, 0 또는 그 값 중 큰 값을 선택
		FoundAbil->Value = FMath::Max(0.0f, FoundAbil->Value + abil.Value);

		RefreshTotalAbil();
	}

}

//여러개 입력시
void UAbilityComponent::server_AddAbilData_Implementation(eAbilCategoryType category, const TArray<FAbilityData>& AbilToApply)
{
	AddAbilData(category, AbilToApply);
}

//단일 입력시
void UAbilityComponent::server_AddAbilDataSingle_Implementation(eAbilCategoryType category, const FAbilityData AbilToApply)
{
	AddAbilData(category, AbilToApply);
}

//TID로 지정
void UAbilityComponent::server_EraseAbilData_Implementation(eAbilCategoryType category, FName TID)
{
	EraseAbilData(category, TID);
}

void UAbilityComponent::server_RegenMP_Implementation()
{
	const FAbilityData* MaxMPData = TotalAbils.FindByKey(eAbilType::ABIL_TYPE_MAX_MP);
	if (!MaxMPData) return;

	const float MaxMP = MaxMPData->Value;

	if (CurrentMP < MaxMP)
	{
		CurrentMP = FMath::Min(CurrentMP + ManaPerSecond * 0.1f, MaxMP);
	}

}

void UAbilityComponent::server_ApplyHP_Implementation(float Amount)
{
	// Max HP 데이터 조회
	FAbilityData* MaxHPData = TotalAbils.FindByKey(eAbilType::ABIL_TYPE_MAX_HP);
	if (!MaxHPData) return; 

	float NewHP = CurrentHP + Amount; // 포션(양수)이든 데미지(음수)이든 모두 합산

	// NewHP를 0 (최소값)과 MaxHP (최대값) 사이로 제한하고 CurrentHP에 할당
	CurrentHP = FMath::Clamp(NewHP, 0.0f, MaxHPData->Value);

	// 변경 알림
	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, GetMaxHP());
	}
}

void UAbilityComponent::server_ApplyMP_Implementation(float Amount)
{
	FAbilityData* MaxMPData = TotalAbils.FindByKey(eAbilType::ABIL_TYPE_MAX_MP);
	if (!MaxMPData) return; 

	float NewMP = CurrentMP + Amount; 

	CurrentMP = FMath::Clamp(NewMP, 0.0f, MaxMPData->Value);
}

void UAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityComponent, DefaultAbils);
	DOREPLIFETIME(UAbilityComponent, ItemEquipAbils);
	DOREPLIFETIME(UAbilityComponent, OwnedAbils);
	DOREPLIFETIME(UAbilityComponent, BuffAbils);
	DOREPLIFETIME(UAbilityComponent, DeBuffAbils);
	DOREPLIFETIME(UAbilityComponent, TotalAbils);
	DOREPLIFETIME(UAbilityComponent, CurrentHP);
	DOREPLIFETIME(UAbilityComponent, CurrentMP);
	DOREPLIFETIME(UAbilityComponent, ManaPerSecond);
}



