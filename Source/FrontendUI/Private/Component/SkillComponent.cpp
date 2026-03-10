// Vince Petrelli All Rights Reserved


#include "Component/SkillComponent.h"
#include "Instance/TableSubsystem.h"
#include "Characters/CC_CharacterBase.h"
#include "Component/AbilityComponent.h"
#include "Net/UnrealNetwork.h"
#include "SkillAction/SkillActionBase.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/InventoryComponent.h"
#include "SkillAction/ProjectileAction.h"
#include "SkillAction/RangedAction.h"
#include "SkillAction/AreaAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DecalComponent.h"
#include "Actor/SkillIndicator.h"
#include "Controllers/CC_PlayerController.h"
#include "Controllers/CC_AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	// 이 컴포넌트가 '기본 객체(CDO)'라면 로직을 실행하지 않고 리턴합니다.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// 안전하게 World가 있는지 확인합니다.
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			CoolDownHandle,
			this,
			&USkillComponent::server_CurrentSkillCoolDownRogic,
			0.1f,   // 0.1초마다 계산
			true
		);
		// ...
	}
	
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
}

TArray<FSkillData> USkillComponent::GetSkillDatas()
{
	return SkillDatas;
}

FSkillTableRow USkillComponent::GetCurrentSkillData()
{
	return CurrentSkillTableData;
}

FName USkillComponent::GetCurrentSkillTID()
{
	return CurrentCastSkillTID;
}

FName USkillComponent::GetNormalSkillTID()
{
	return NormalActionTID;
}

FName USkillComponent::GetSubNormalSkillTID()
{
	return SubNormalActionTID;
}

void USkillComponent::SetCurrentSkillTID(FName tid)
{
	CurrentCastSkillTID = tid;
}


FSkillData* USkillComponent::IsUsedSkill(FName& skillTID)
{
	UTableSubsystem* tableSub = UTableSubsystem::Get(this);
	if (nullptr == tableSub)
		return nullptr;

	FSkillTableRow* skillRow = tableSub->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, skillTID);
	if (nullptr == skillRow)
		return nullptr;

	FSkillData* foundSkill = FindSkill(skillTID);
	if (nullptr == foundSkill)
		return nullptr;

	if (0 != foundSkill->CurrentCoolDown)
		return nullptr;

	return foundSkill;
}

void USkillComponent::SetSkillData(FName& skillTID)
{
	UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
	if (nullptr == tableSubsystem)
		return;

	FSkillTableRow* skillRow = tableSubsystem->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, skillTID);
	if (nullptr == skillRow)
		return;

	FSkillData* foundSkill = FindSkill(skillTID);
	if (foundSkill)
		return;
	
	TSubclassOf<USkillActionBase> SkillAction = skillRow->SkillActionClass;
	float SkillRange = 0.0f;
	if (SkillAction->IsChildOf(UAreaAction::StaticClass()))
	{
		SkillRange = skillRow->AreaActionData.MaxDistance;
	}
	else if (SkillAction->IsChildOf(UProjectileAction::StaticClass()))
	{
		SkillRange = skillRow->ProjectileBehaviorData.Range - 100.f;
	}
	else if (SkillAction->IsChildOf(URangedAction::StaticClass()))
	{
		SkillRange = skillRow->RangedEffectBehaviorData.Range;
	}

	SkillDatas.Add(FSkillData(skillTID, skillRow->CoolDown, skillRow->CostMana, SkillRange));

	if (skillRow->Type == eSkillType::SKILL_TYPE_NORMAL_ATTACK)
	{
		if (NormalActionTID.IsNone())
			NormalActionTID = skillTID;
		else
			UE_LOG(LogTemp, Error, TEXT("Player NormalSkillAction 중복 데이터가 들어왔습니다"));
	}
	else if (skillRow->Type == eSkillType::SKILL_TYPE_SUB_NORMAL_ATTACK)
	{
		if (SubNormalActionTID.IsNone())
			SubNormalActionTID = skillTID;
		else
			UE_LOG(LogTemp, Error, TEXT("Player SubNormalSkillAction 중복 데이터가 들어왔습니다"));
	}
}


FSkillData* USkillComponent::FindSkill(FName& skillTID)
{
	return SkillDatas.FindByKey(skillTID);
}


void USkillComponent::SetSkillSlot(int32 SlotNum, FName SkillTID)
{
	bool bFound = false;

	for (auto& Slot : SkillSlots)
	{
		if (Slot.SlotNum == SlotNum)
		{
			Slot.SkillTID = SkillTID;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		SkillSlots.Add(FSkillSlotData{ SlotNum, SkillTID });
	}
}

void USkillComponent::EraseSkillSlot(int32 SlotNum)
{
	int32 slotnum = SlotNum;
	SkillSlots.RemoveAll([slotnum](const FSkillSlotData& Slot) {
		return Slot.SlotNum == slotnum;
		});
}

void USkillComponent::RefreshSkillCoolDown(FName& skillID)
{
	FSkillData* foundSkill = FindSkill(skillID);
	if (nullptr == foundSkill)
		return;

	foundSkill->CurrentCoolDown = 0;
}

void USkillComponent::ClearCastSkill()
{
	CurrentCastSkillTID = FName();
	CurrentSkillTableData = FSkillTableRow();
	ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(GetOwner());
	if (nullptr == Character)
		return;
	Character->SetIsCastingCheck(false);
	Character->SetSaveSkillLocation(FVector::ZeroVector);
}

void USkillComponent::CurrentSkillResetCoolDown()
{
	for (auto& it : SkillDatas)
	{
		if (CurrentCastSkillTID == it.TID)
			it.CurrentCoolDown = it.CoolDown;
	}
}

FName USkillComponent::GetSkillTIDInSlot(int32 SlotNum) const
{
	// SkillSlots 배열을 순회하며 해당 슬롯 번호를 찾음
	for (const FSkillSlotData& Slot : SkillSlots)
	{
		if (Slot.SlotNum == SlotNum)
		{
			return Slot.SkillTID;
		}
	}
	return NAME_None; // 없으면 None 반환
}

bool USkillComponent::SkillAction(FName skillID)
{
	if (!CurrentCastSkillTID.IsNone())
		return false;

	server_SkillAction(skillID);

	return true;
}

void USkillComponent::CancelCurrentSkill()
{
	server_CancelCurrentSkill();
}

void USkillComponent::CreateSkillIndicator(FSkillTableRow* skilltable, bool IsPlayer)
{
	if (SkillDecalActor)
	{
		SkillDecalActor->Destroy();
		SkillDecalActor = nullptr;
	}
	eSkillType Type = skilltable->Type;
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	FVector OwnerForward = GetOwner()->GetActorForwardVector();

	//|| Type == eSkillType::SKILL_TYPE_NORMAL_ATTACK

	if (Type == eSkillType::SKILL_TYPE_AREA || Type == eSkillType::SKILL_TYPE_SUB_NORMAL_ATTACK )
	{
		float ConeHalfAngle = skilltable->AreaActionData.ConeHalfAngle;


		if (ConeHalfAngle == 0)
		{
			float MinDist = skilltable->AreaActionData.MinDistance;
			float MaxDist = skilltable->AreaActionData.MaxDistance;
			float Width = skilltable->AreaActionData.Range;
			//실제 세로길이
			float Height = MaxDist - MinDist;
			float HalfWidth = Width * 0.5f;
			float HalfHeight = Height * 0.5f;

			float DistanceToCenter = MinDist + (Height * 0.5f);

			FVector DecalLocation = OwnerLocation + (OwnerForward * DistanceToCenter);

			ASkillIndicator* Indicator = GetWorld()->SpawnActor<ASkillIndicator>(DecalLocation, OwnerForward.Rotation());
			if (nullptr == Indicator)
				return;
	
			Indicator->SetupIndicator(eDecalType::DECAL_BOX, FVector(1000.f, HalfWidth, HalfHeight));
			SkillDecalActor = Indicator;
		}
		else
		{
			FVector DecalLocation = OwnerLocation + (OwnerForward * skilltable->AreaActionData.MinDistance);
			ASkillIndicator* Indicator = GetWorld()->SpawnActor<ASkillIndicator>(DecalLocation, OwnerForward.Rotation());
			if (nullptr == Indicator)
				return;

			float Angle = skilltable->AreaActionData.ConeHalfAngle * 2.0f;
			float Radian = FMath::DegreesToRadians(Angle);
			float Radius = skilltable->AreaActionData.MaxDistance - skilltable->AreaActionData.MinDistance;
			Indicator->SetupIndicator(eDecalType::DECAL_CONE, FVector(1000.f, Radius, Radius));
			Indicator->SetupConeAngle(Radian);
			SkillDecalActor = Indicator;
		}
	}
	else if ( Type == eSkillType::SKILL_TYPE_RANGED)
	{
		ACC_CharacterBase* Character = Cast<ACC_CharacterBase>(GetOwner());
		if (nullptr == Character)
			return;
		FVector TargetLocation = FVector::ZeroVector;

		float MaxRange = skilltable->RangedEffectBehaviorData.Range;
		float Radius = skilltable->RangedEffectBehaviorData.EffectRadius;
		if (IsPlayer)
		{
			ACC_PlayerController* PlayerController = Cast<ACC_PlayerController>(Character->GetController());
			if (nullptr == PlayerController)
				return;
			
			// 플레이어 (마우스 커서의 월드 평면 교차점 사용)
			FVector WorldLocation; // 카메라 위치
			FVector WorldDirection; // 마우스 커서 방향 벡터

			// 마우스 위치를 월드 좌표로 디프로젝트
			PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

			// 시전자 Z축 높이(OwnerLocation.Z)를 지나는 평면과 광선(Ray)의 교차점을 계산합니다.
			// 이 로직은 지형과 관계없이 마우스 커서 아래의 3D 공간 위치를 결정합니다.

			if (FMath::IsNearlyZero(WorldDirection.Z))
			{
				// 수평에 가까울 경우, 시전자 정면 최대 사거리로 폴백 (Fall-back) 처리
				TargetLocation = OwnerLocation + Character->GetActorForwardVector() * MaxRange;
			}
			else
			{
				// (Z_owner - Z_cam) / D_ray.z : 광선이 평면(Z_owner)에 도달하는 데 필요한 거리(t) 계산
				float DistanceToPlane = (OwnerLocation.Z - WorldLocation.Z) / WorldDirection.Z;

				// WorldLocation에서 WorldDirection으로 t만큼 이동한 지점이 목표 위치입니다.
				TargetLocation = WorldLocation + WorldDirection * DistanceToPlane;
			}
		}
		else
		{
			ACC_AIController* controller = Cast<ACC_AIController>(Character->GetController());
			if (nullptr == controller)
				return;
			ACC_MonsterCharacter* monster = Cast<ACC_MonsterCharacter>(Character);
			if (nullptr == monster)
				return;

			ACC_CharacterBase* TargetOwner = monster->GetAggroTarget();
			if (nullptr == TargetOwner)
				return;

			TargetLocation = monster->GetSaveSkillLocation();

			FVector DirectionToTarget = TargetLocation - OwnerLocation;
			DirectionToTarget.Z = 0.0f;
		}

		TargetLocation.Z += 300.f;
		ASkillIndicator* Indicator = GetWorld()->SpawnActor<ASkillIndicator>(TargetLocation, FRotator::ZeroRotator);
		if (nullptr == Indicator)
			return;

		Indicator->SetupIndicator(eDecalType::DECAL_CIRCLE, FVector(1000.f, (Radius), (Radius)));
		SkillDecalActor = Indicator;
	}
}

void USkillComponent::EraseSkillIndicator()
{
	if (SkillDecalActor)
		SkillDecalActor->Destroy();

	SkillDecalActor = nullptr;
}


void USkillComponent::server_CurrentSkillCoolDownRogic_Implementation()
{

	const float Delta = 0.1f; // Timer 주기

	for (FSkillData& SkillData : SkillDatas)
	{
		if (SkillData.CurrentCoolDown > 0.f)
		{
			SkillData.CurrentCoolDown -= Delta;
			if (SkillData.CurrentCoolDown < 0.f)
			{
				SkillData.CurrentCoolDown = 0.f;
			}
		}
	}
}

void USkillComponent::server_SkillAction_Implementation(FName SkillTID)
{
	FSkillData* skilldata = IsUsedSkill(SkillTID);
	if (skilldata == nullptr)
		return;

	UTableSubsystem* tablesystem = UTableSubsystem::Get(this);

	ACC_CharacterBase* OwnerCharacter = Cast<ACC_CharacterBase>(GetOwner());
	if (nullptr == tablesystem || nullptr == OwnerCharacter)
		return;
	
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(OwnerCharacter);

	if (player != nullptr)
	{
		FName PlayerNormalSkillAction = GetNormalSkillTID();
		FName PlayerSubNormalSkillAction = GetSubNormalSkillTID();
		if (SkillTID != PlayerNormalSkillAction && SkillTID != PlayerSubNormalSkillAction)

		{
			bool bIsPlayerSkillSlot = false;
			for (auto& it : SkillSlots)
			{
				if (it.SkillTID == SkillTID)
				{
					bIsPlayerSkillSlot = true;
					break;
				}
			}
			if (!bIsPlayerSkillSlot)
				return;
		}
	}
	
	FSkillTableRow* skilltable = tablesystem->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, SkillTID);

	//Projectile , Ranged 두 종류의 스킬은 총알을 1발씩 소모한다. 총알 체크로직
	if (nullptr != player)
	{
		//마나감소, 현재쿨타임 증가는 Action에서 실행
		if (skilltable->CostMana > OwnerCharacter->GetMP())
			return;

		TSubclassOf<USkillActionBase> SkillAction = skilltable->SkillActionClass;
		if (SkillAction->IsChildOf(UProjectileAction::StaticClass())) //|| SkillAction->IsChildOf(URangedAction::StaticClass()))
		{
			int32 CurrentAmmoCount = player->GetCurrentAmmoCount();
			if (CurrentAmmoCount == 0)
				return;
		}
	}
	else
	{
		ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(OwnerCharacter);
		if (nullptr == Monster)
			return;


		//임시로 이렇게 로직을 짬 wizard 의 Explosion 스킬은 몬스터 몸에서 나가야 해서 불가피하게 로직 추가함 추후에 수정
		if (SkillTID != FName("Stage_01_Wizard_Explosion"))
			Monster->SetSaveSkillLocation(Monster->GetAggroTarget()->GetActorLocation());
		else
		{
			Monster->SetSaveSkillLocation(Monster->GetActorLocation());
		}
			
	}

	if (nullptr == skilltable || !skilltable->SkillActionClass.Get())
		return;

	OwnerCharacter->SetIsCastingCheck(true);
	//데칼 액터 생성
	//몬스터만 데칼 생성되게끔 막아둠
	if(nullptr == player)
		CreateSkillIndicator(skilltable, player != nullptr ? true : false);

	CurrentCastSkillTID = SkillTID;
	CurrentSkillTableData = *skilltable;


	UAnimMontage* Montage = OwnerCharacter->GetSkillMontage(skilltable->MontageID);
	

	OwnerCharacter->MontageAction(Montage, true);
}

void USkillComponent::server_RefeeshSkillCollDown_Implementation(FName SkillTID)
{
	RefreshSkillCoolDown(SkillTID);
}

void USkillComponent::server_SetSkillData_Implementation(FName TID)
{
	SetSkillData(TID);
}

void USkillComponent::server_SetSkillSlot_Implementation(int32 slot, FName TID)
{
	FSkillData* skilldata = FindSkill(TID);
	if (nullptr == skilldata)
		return;

	SetSkillSlot(slot, TID);
}

void USkillComponent::server_EraseSkillSlot_Implementation(int32 slot)
{
	EraseSkillSlot(slot);
}

void USkillComponent::server_CancelCurrentSkill_Implementation()
{
	ClearCastSkill();

	ACC_CharacterBase* character = Cast<ACC_CharacterBase>(GetOwner());
	
	if (character)
	{
		character->Multicast_StopMontage(0.2f);
	}
}

void USkillComponent::Server_ExecuteSkillAction_Implementation()
{
	ACC_CharacterBase* OwnerCharacter = Cast<ACC_CharacterBase>(GetOwner());
	if (!OwnerCharacter) return;

	if (CurrentSkillTableData.SkillActionClass)
	{
		USkillActionBase* ActionInstance = NewObject<USkillActionBase>(this, CurrentSkillTableData.SkillActionClass);
		if (ActionInstance)
		{
			ActionInstance->SkillAction(OwnerCharacter);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("서버: CurrentSkillTableData가 비어있습니다!"));
	}
}

void USkillComponent::Server_RequestReload_Implementation()
{

	// 1. 플레이어 및 상태 확인
	ACC_PlayerCharacter* player = Cast<ACC_PlayerCharacter>(GetOwner());
	if (nullptr == player) return;

	ACC_PlayerState* PlayerState = player->GetPlayerState<ACC_PlayerState>();
	if (!PlayerState) return;

	UInventoryComponent* Inventory = PlayerState->GetInventory();
	if (nullptr == Inventory) return;

	// 2. 현재 장전 상태 확인
	int32 MaxAmmo = player->GetMaxAmmoCount();      // 30
	int32 CurrentAmmo = player->GetCurrentAmmoCount();

	// 이미 탄창이 꽉 찼으면 리턴
	if (CurrentAmmo >= MaxAmmo) return;

	// 3. 필요한 탄약량 계산 (30 - 현재 잔탄)
	int32 AmmoNeeded = MaxAmmo - CurrentAmmo;

	// 4. 인벤토리에서 'Ammo' 아이템 찾기
	FItemData InventoryAmmoData = Inventory->GetAmmoData();

	// 인벤토리에 탄약이 없으면 리턴
	if (InventoryAmmoData.Count <= 0 || InventoryAmmoData.TID.IsNone())
	{
		// (옵션) 탄약 부족 메시지 출력 등
		return;
	}

	// 5. 실제 장전 가능한 양 계산 (필요량 vs 보유량 중 작은 값)
	int32 ReloadAmount = FMath::Min(AmmoNeeded, InventoryAmmoData.Count);

	if (ReloadAmount > 0)
	{
		// 6. 인벤토리에서 차감 (실시간 반영)
		// UID와 차감할 개수를 전달하여 인벤토리에서 제거
		Inventory->TryEraseStackItem(InventoryAmmoData.TID, InventoryAmmoData.UID, ReloadAmount);

		// 7. 플레이어 탄창 채우기
		// SetCurrentAmmoCount는 내부적으로 += 연산을 수행하도록 되어 있으므로 추가할 양만 넘김
		player->SetCurrentAmmoCount(ReloadAmount);

		UE_LOG(LogTemp, Log, TEXT("Reload Complete. Added: %d, Current Mag: %d"), ReloadAmount, player->GetCurrentAmmoCount());
	}
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillComponent, SkillSlots);
	DOREPLIFETIME(USkillComponent, SkillDatas);
	DOREPLIFETIME(USkillComponent, CurrentCastSkillTID);
	DOREPLIFETIME(USkillComponent, NormalActionTID);
	DOREPLIFETIME(USkillComponent, SubNormalActionTID);
}