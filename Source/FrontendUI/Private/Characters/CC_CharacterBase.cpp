// Vince Petrelli All Rights Reserved


#include "Characters/CC_CharacterBase.h"
#include "Actor/Projectile/ProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h" 
#include "GameFramework/CharacterMovementComponent.h" 
#include "UObject/ConstructorHelpers.h"
#include "Input/DefaultInputSystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "Component/AbilityComponent.h"
#include "Instance/TableSubsystem.h"
#include "Interface/AttackStatsSource.h"
#include "Component/SkillComponent.h"
#include "Engine/OverlapResult.h"
#include "Actor/RangedEffect/RangedEffectBase.h"
#include "CollisionShape.h"  
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Table/TableDatas.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/CC_PlayerState.h"
#include "Component/EquipmentComponent.h" 
#include "Widgets/Options/Widget_FloatingDamage.h"
#include "Components/WidgetComponent.h"
#include "Widgets/Options/HealthBarWidget.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ACC_CharacterBase::ACC_CharacterBase()
{
	// **[제거됨]** 카메라, 스프링암 설정 (ACC_PlayerCharacter로 이동)
		// **[제거됨]** 캐릭터 이동 관련 설정 (ACC_PlayerCharacter로 이동)
		// **[제거됨]** 스프링 암 관련 설정 (ACC_PlayerCharacter로 이동)

	PrimaryActorTick.bCanEverTick = true;

	// **[제거됨]** 입력 시스템 객체 생성 (ACC_PlayerCharacter로 이동)

	// AbilityComponent를 생성 (유지)
	Ability = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability"));
	// SkillComponent 생성 (유지)
	Skill = CreateDefaultSubobject<USkillComponent>(TEXT("Skill"));
	// 기본 설정: NPC/몬스터는 컨트롤러 회전 사용하지 않음
	bUseControllerRotationYaw = false;

	// 무기 설정
	Weapon_R_MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_R_MeshComp"));
	Weapon_R_MeshComp->SetupAttachment(GetMesh(), TEXT("Weapon_RSocket")); // 소켓 이름은 에디터 세팅에 맞게 수정
	Weapon_R_MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 무기 자체 충돌은 보통 끔

	// 무기 설정
	Weapon_L_MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_L_MeshComp"));
	Weapon_L_MeshComp->SetupAttachment(GetMesh(), TEXT("Weapon_LSocket")); // 소켓 이름은 에디터 세팅에 맞게 수정
	Weapon_L_MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 무기 자체 충돌은 보통 끔


	// 헬멧 설정
	HelmetMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));
	HelmetMeshComp->SetupAttachment(GetMesh(), TEXT("headSocket")); // 머리 소켓 필요
	HelmetMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 갑옷 설정 (등이나 가슴 소켓에 붙인다면)
	ArmorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorMesh"));
	ArmorMeshComp->SetupAttachment(GetMesh(), TEXT("spineSocket")); // 척추 소켓 필요
	ArmorMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 위젯 컴포넌트 생성 (체력바)
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetCanEverAffectNavigation(false);
	HPBarWidget->SetupAttachment(GetRootComponent());

	// 위치 설정 (머리 위로)
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));

	// 중요: 위젯을 월드 공간이 아닌 '스크린' 공간에 그리면 항상 카메라를 정면으로 바라봅니다.
	// (빌보드 효과). 3D 느낌을 원하면 World, 항상 잘 보이게 하려면 Screen.
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// 크기 설정 (WBP의 Size Box 크기와 맞추는 게 좋음)
	HPBarWidget->SetDrawSize(FVector2D(150.0f, 10.0f));


	bReplicates = true;
	SetReplicateMovement(true);
	SetReplicatingMovement(true);
}

TObjectPtr<UAnimBase> ACC_CharacterBase::GetAnimBase()
{
	return Anim;
}

TObjectPtr<UAbilityComponent> ACC_CharacterBase::GetAbilityComp()
{
	return Ability;
}

TObjectPtr<USkillComponent> ACC_CharacterBase::GetSkillComp()
{
	return Skill;
}

void ACC_CharacterBase::SetIsCastingCheck(bool IsCasting)
{
	bIsCastingSkill = IsCasting;
}

UAnimMontage* ACC_CharacterBase::GetSkillMontage(FName montageID)
{
	UAnimMontage* montage = nullptr;
	for (auto& it : SkillMontages)
	{
		if (it.MontageID == montageID)
			montage = it.MontageAsset;
	}

	return montage;
}

void ACC_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
		
	UpdateEquipmentVisuals();

	// 1. 위젯 컴포넌트에서 실제 위젯 객체 가져오기
	if (HPBarWidget)
	{
		// 생성된 위젯을 C++ 클래스로 캐스팅
		UHealthBarWidget* HealthWidget = Cast<UHealthBarWidget>(HPBarWidget->GetUserWidgetObject());

		if (HealthWidget && Ability)
		{
			// 2. 초기값 설정 (처음에 꽉 찬 상태 혹은 현재 상태로 갱신)
			HealthWidget->UpdateHP(Ability->GetHP(), Ability->GetMaxHP());

			// 3. 델리게이트 연결 (C++에서 바인딩)
			// AbilityComponent의 OnHPChanged가 방송되면 -> HealthWidget의 UpdateHP가 실행됨
			Ability->OnHPChanged.AddDynamic(HealthWidget, &UHealthBarWidget::UpdateHP);
		}
	}
}

void ACC_CharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACC_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ACC_CharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	const FAbilityData* HPData = GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_MAX_HP);
	if (nullptr == HPData)
		return 0.0f;

	
	AProjectileBase* projectile = Cast<AProjectileBase>(DamageCauser);
	ARangedEffectBase* RangedEffect = Cast<ARangedEffectBase>(DamageCauser);
	float ResultDamage = 0.0f;
	if (projectile || RangedEffect)
	{
		float ArmorPen = 0.0f;
		if (projectile)
			ArmorPen = projectile->PercentArmorPen;
		else if (RangedEffect)
			ArmorPen = RangedEffect->PercentArmorPen;

		ArmorPen *= 0.01;

		float MyArmor = 0.0f;

		const FAbilityData* AbilityData = GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_DEFENCE);
		if (nullptr == AbilityData)
			MyArmor = 0.0f;
		else
			MyArmor = AbilityData->Value;

		float EffectiveArmor = MyArmor * (1.0f - ArmorPen);

		EffectiveArmor = FMath::Max(0.0f, EffectiveArmor);
		// 피해 감소율 계산 (일반적인 RPG 공식)
		// 피해 감소율 = 유효 방어력 / (유효 방어력 + 상수 100)
		float DamageReductionRatio = EffectiveArmor / (EffectiveArmor + 100.0f);

		DamageReductionRatio = FMath::Clamp(DamageReductionRatio, 0.0f, 0.9f);

		// 최종 피해량 계산
		ResultDamage = DamageAmount * (1.0f - DamageReductionRatio);

		//최소데미지 1 보장
		if (ResultDamage < 1.0f)
			ResultDamage = 1.0f;

		// 5. HP 적용 및 반환
		// ApplyHP는 HP를 감소시키는 함수라고 가정합니다.
		ApplyHP(-ResultDamage);

	}
	else
	{
		ResultDamage = FMath::Max(1.0f, DamageAmount);

		// 5. HP 적용 및 반환
		// ApplyHP는 HP를 감소시키는 함수라고 가정합니다.
		ApplyHP(-ResultDamage);
	}

	bool bIsCritical = false;

	if (HasAuthority() && ResultDamage > 0.0f && EventInstigator)
	{
		// 공격자의 Pawn 가져오기
		APawn* AttackerPawn = EventInstigator->GetPawn();
		if (AttackerPawn)
		{
			ACC_CharacterBase* AttackerCharacter = Cast<ACC_CharacterBase>(AttackerPawn);
			if (AttackerCharacter)
			{
				// 피격시 호출
				Multicast_ShowDamageText(ResultDamage, bIsCritical, this);
			}
		}
	}

	return ResultDamage;
}

void ACC_CharacterBase::Multicast_ShowDamageText_Implementation(float DamageAmount, bool bIsCritical, AActor* DamagedActor)
{
	if (!IsValid(DamagedActor)) return;

	// 중요: NetMulticast는 모든 클라이언트에서 실행되므로,
	//      실제로 화면을 보고 있는 로컬 플레이어인 경우에만 위젯을 생성하도록 확인해야 합니다.
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !PC->IsLocalController())
	{
		// 로컬 플레이어가 아니면 아무것도 하지 않고 함수를 종료합니다.
		return;
	}

	// DamageTextWidgetClass는 이 함수를 실행하는 객체(피격당한 캐릭터)의 것을 사용합니다.
	if (!DamageTextWidgetClass)
	{
		return;
	}

	UWidget_FloatingDamage* DamageWidget = CreateWidget<UWidget_FloatingDamage>(PC, DamageTextWidgetClass);
	if (DamageWidget)
	{
		DamageWidget->AddToViewport(100);
		// 애니메이션 재생 노드를 블루프린트에 추가했는지 다시 한번 확인해주세요!
		DamageWidget->Initialize(DamageAmount, bIsCritical, DamagedActor);
	}
}

float ACC_CharacterBase::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
	const FAbilityData* HPData = GetAbility(eAbilCategoryType::ABIL_CATEGORY_TOTAL, eAbilType::ABIL_TYPE_MAX_HP);
	if (nullptr == HPData)
		return 0.0f;


	ApplyHP(-DamageAmount);
		
	if (HasAuthority() && DamageAmount > 0.0f)
	{
		// DamageCauser가 캐릭터라면 그 캐릭터가 공격자임
		Multicast_ShowDamageText(DamageAmount, false, this);
	}

	return DamageAmount;
}

void ACC_CharacterBase::SetCharacterData(FName& tid)
{


}

void ACC_CharacterBase::SetSaveSkillLocation(FVector Location)
{
	SavedSkillLocation = Location;
}

FVector ACC_CharacterBase::GetSaveSkillLocation()
{
	return SavedSkillLocation;
}

void ACC_CharacterBase::AddAbilData(eAbilCategoryType type, const TArray<FAbilityData>& abils)
{
	if (false == IsValid(Ability))
		return;

	Ability->server_AddAbilData(type, abils);
}

void ACC_CharacterBase::AddAbilData(eAbilCategoryType type, const FAbilityData& abil)
{
	if (false == IsValid(Ability))
		return;

	Ability->server_AddAbilDataSingle(type, abil);
}
void ACC_CharacterBase::EraseAbilData(eAbilCategoryType category, FName TID)
{
	if (false == IsValid(Ability))
		return;

	Ability->server_EraseAbilData(category, TID);
}

void ACC_CharacterBase::SetDefaultAbil(const TArray<FAbilityData>& abils)
{
	if (false == IsValid(Ability))
		return;
	
	Ability->SetDefaultAbil(abils);
}

void ACC_CharacterBase::ApplyDefaultAbil(const FAbilityData& abil)
{
	if (false == IsValid(Ability))
		return;

	Ability->ApplyDefaultAbil(abil);
}
void ACC_CharacterBase::HandleAttackAnimationEnded(bool bInterruped)
{
	//Character에서는 구현만 해 둔거임
	//몬스터쪽에서 사용하기 위해 구현 (AnimBase 쪽 몽타주 종료 시점에 실행됨)
}

const FAbilityData* ACC_CharacterBase::GetAbility(eAbilCategoryType category, eAbilType type)
{
	if (false == IsValid(Ability))
		return nullptr;

	return Ability->GetAbil(category, type);
}

void ACC_CharacterBase::ApplyHP(float hp)
{
	Ability->ApplyHP(hp);
}
void ACC_CharacterBase::ApplyMP(float mp)
{
	Ability->ApplyMP(mp);
}


float ACC_CharacterBase::GetHP()
{
	return Ability->GetHP();
}
float ACC_CharacterBase::GetMP()
{ 
	return Ability->GetMP();
}

float ACC_CharacterBase::GetHPRatio_Safe()
{
	// Ability 컴포넌트가 아직 없으면 0.0 반환 (오류 방지)
	if (Ability == nullptr)
	{
		return 0.0f;
	}

	return Ability->GetHPRatio();
}

float ACC_CharacterBase::GetMPRatio_Safe()
{
	// Ability 컴포넌트가 아직 없으면 0.0 반환
	if (Ability == nullptr)
	{
		return 0.0f;
	}

	return Ability->GetMPRatio();
}

void ACC_CharacterBase::CancelCurrentSkill()
{
	Skill->CancelCurrentSkill();
}

void ACC_CharacterBase::SetSkillData(FName& tid)
{
	if (false == IsValid(Skill))
		return;

	Skill->server_SetSkillData(tid);
}

FSkillData* ACC_CharacterBase::IsUsedSkill(FName& skillTID)
{
	if (false == IsValid(Skill))
		return nullptr;

	return Skill->IsUsedSkill(skillTID);
}
	
bool ACC_CharacterBase::SkillAction(FName& skillID)
{
	if (false == IsValid(Skill))
		return false;

	Skill->SkillAction(skillID);

	return true;
}



void ACC_CharacterBase::RefreshSkillCoolDown(FName& skillID)
{
	if (false == IsValid(Skill))
		return;

	Skill->server_RefeeshSkillCollDown(skillID);

}

void ACC_CharacterBase::AddReward(FRewardTableRow* row)
{

}

void ACC_CharacterBase::SetTagNames(TArray<FName>& tagIDs)
{
	for (auto& it : tagIDs)
		Tags.Add(it);
}

void ACC_CharacterBase::EquipItem(FItemEquipData& data)
{

}

void ACC_CharacterBase::Multicast_StopMontage_Implementation(float InblendOutTIme)
{
	if (Anim)
	{
		Anim->Montage_Stop(0.2f); // 0.2초 BlendOut 시간
	}
}

void ACC_CharacterBase::Multicast_PlayMontage_Implementation(UAnimMontage* montage, bool SkillCast)
{
	if (false == IsValid(Anim))
		return;

	Anim->PlayMontage(montage, SkillCast);
}

void ACC_CharacterBase::MontageAction(UAnimMontage* montage, bool IsSkillCast)
{
	if (false == IsValid(Anim))
		return;

	Multicast_PlayMontage(montage, IsSkillCast);
}

FSkillTableRow ACC_CharacterBase::GetCurrentSkillData()
{
	return Skill->GetCurrentSkillData();
}

FName ACC_CharacterBase::GetCurrentSkillTID()
{
	return Skill->GetCurrentSkillTID();
}

void ACC_CharacterBase::SetSkillSlot(int32 slot, FName TID)
{
	if (false == IsValid(Skill))
		return;
	Skill->server_SetSkillSlot(slot, TID);
}

void ACC_CharacterBase::EraseSkillSlot(int32 slot)
{
	if (false == IsValid(Skill))
		return;
	Skill->server_EraseSkillSlot(slot);
}

void ACC_CharacterBase::SetAnimationInstance(FName& animTID)
{
	USkeletalMeshComponent* mesh = GetMesh();
	if (nullptr == mesh)
		return;

	UTableSubsystem* tableSub = UTableSubsystem::Get(this);
	if (nullptr == tableSub)
		return;

	FAnimationTableRow* animRow = tableSub->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, animTID);
	if (nullptr == animRow)
		return;

	mesh->SetAnimInstanceClass(animRow->Animation);

}

void ACC_CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACC_CharacterBase, Anim);
	DOREPLIFETIME(ACC_CharacterBase, Ability);
	DOREPLIFETIME(ACC_CharacterBase, Skill);
	DOREPLIFETIME(ACC_CharacterBase, SkillMontages);
}

void ACC_CharacterBase::UpdateWeaponVisual(const FName& ItemTID, eItemEquipType Type)
{
	switch (Type)
	{
	case eItemEquipType::WEAPON_R:
		Weapon_R_MeshSetting(ItemTID);
		break;
	case eItemEquipType::WEAPON_L:
		Weapon_L_MeshSetting(ItemTID);
		break;
	case eItemEquipType::HELMET:
		Helmet_MeshSetting(ItemTID);
		break;
	case eItemEquipType::ARMOR:
		Armor_MeshSetting(ItemTID);
		break;
	default:
		break;
	}
}

void ACC_CharacterBase::UpdateEquipmentVisuals()
{
	// PlayerState를 통해 장비 컴포넌트 접근
	ACC_PlayerState* PS = GetPlayerState<ACC_PlayerState>();
	if (!PS) return;
	UEquipmentComponent* EquipComp = PS->GetEquipment();
	if (!EquipComp) return;

	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	const TArray<FItemEquipSlotData>& Slots = EquipComp->GetEquipSlots();

	// 1. 초기화 (다 벗기기)
	Weapon_R_MeshComp->SetStaticMesh(nullptr);
	Weapon_L_MeshComp->SetStaticMesh(nullptr);
	HelmetMeshComp->SetStaticMesh(nullptr);
	ArmorMeshComp->SetStaticMesh(nullptr);

	// 2. 장착된 것만 입히기
	for (const auto& Slot : Slots)
	{
		FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, Slot.ItemEquipData.TID);
		if (!Row) continue;

		switch (Slot.Type)
		{
		case eItemEquipType::WEAPON_R:
			Weapon_R_MeshComp->SetStaticMesh(Row->Mesh);
			break;
		case eItemEquipType::WEAPON_L:
			Weapon_L_MeshComp->SetStaticMesh(Row->Mesh);
			break;
		case eItemEquipType::HELMET:
			HelmetMeshComp->SetStaticMesh(Row->Mesh);
			break;
		case eItemEquipType::ARMOR:
			ArmorMeshComp->SetStaticMesh(Row->Mesh);
			break;
		}
	}	
}

void ACC_CharacterBase::Weapon_R_MeshSetting(const FName& TID)
{
	if (TID.IsNone())
	{
		Weapon_R_MeshComp->SetStaticMesh(nullptr);
		return;
	}
	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	if (FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, TID))
	{
		Weapon_R_MeshComp->SetStaticMesh(Row->Mesh);
	}
}

void ACC_CharacterBase::Weapon_L_MeshSetting(const FName& TID)
{
	if (TID.IsNone())
	{
		Weapon_L_MeshComp->SetStaticMesh(nullptr);
		return;
	}
	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	if (FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, TID))
	{
		Weapon_L_MeshComp->SetStaticMesh(Row->Mesh);
	}
}

void ACC_CharacterBase::Helmet_MeshSetting(const FName& TID)
{
	if (TID.IsNone())
	{
		HelmetMeshComp->SetStaticMesh(nullptr);
		return;
	}
	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	if (FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, TID))
	{
		HelmetMeshComp->SetStaticMesh(Row->Mesh);
	}
}

void ACC_CharacterBase::Armor_MeshSetting(const FName& TID)
{
	if (TID.IsNone())
	{
		ArmorMeshComp->SetStaticMesh(nullptr);
		return;
	}
	UTableSubsystem* TableSub = UTableSubsystem::Get(this);
	if (!TableSub) return;

	if (FItemTableRow* Row = TableSub->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, TID))
	{
		ArmorMeshComp->SetStaticMesh(Row->Mesh);
	}
}
