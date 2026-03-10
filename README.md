# Data-Driven Game System Architecture (Unreal Engine / C++)

TL;DR

Unreal Engine C++ 기반의 Data-Driven 게임 시스템 아키텍처
Skill / Ability / Inventory / Spawn 시스템을 컴포넌트 기반으로 분리
모든 게임 콘텐츠를 DataTable 중심으로 확장 가능
멀티플레이 환경을 고려한 서버 권한 구조 설계

## Overview
본 프로젝트는 Unreal Engine 기반의 게임에서  
스킬, 어빌리티, 아이템, 인벤토리 등 핵심 시스템을  
데이터 테이블 중심(Data-Driven) 으로 설계·구현한 프로젝트입니다.

복잡한 내부 로직은 시스템 레벨에서 캡슐화하고,  
상위 계층(UI, 캐릭터 로직)은 단일 API 호출만으로 기능을 사용할 수 있도록 설계하는 것을 목표로 했습니다.

## GAS 를 쓰지 않은 이유
본 프로젝트는 GAS를 대체하기 위함이 아니라,
경량 프로젝트 및 학습 목적에서
구조를 직접 설계·구현하는 데 목적이 있습니다.

## Demo Video

- Gameplay Demo (YouTube):  
https://youtu.be/cVC-oAl0Xw0

0:00 DataTable-driven Stat & Skill Edit
0:57 In-game Gameplay Demo

---

## Core Design Goals
- 데이터 변경만으로 게임 밸런스 및 콘텐츠 확장 가능
- UI / Gameplay Logic 간 의존성 최소화
- 멀티플레이(Replication)를 고려한 서버 권한 구조
- 동료 개발자가 내부 구조를 몰라도 안전하게 사용할 수 있는 인터페이스 제공

---

## System Architecture

### Data-Driven Table Architecture
- 모든 캐릭터 / 스킬 / 어빌리티 / 아이템은 DataTable 기반으로 정의
- `TableSubsystem`을 통해 중앙 집중식 접근 제공
- 코드 수정 없이 데이터 확장 가능하도록 설계

---

## 1. CharacterBase

CharacterBase는 Player, Monster, NPC가 공통으로 사용하는
최상위 캐릭터 베이스 클래스입니다.

이 클래스는 직접 게임 시스템 로직을 구현하지 않고,
Ability, Skill, Inventory 등의 핵심 시스템 컴포넌트를 조합(Composition) 하는 역할만 담당합니다.

이를 통해 캐릭터 타입이 늘어나더라도
시스템 로직은 재사용되고,
캐릭터 클래스는 행동 정의에만 집중할 수 있도록 설계했습니다.

---

## 2. 캐릭터 클래스들과 결합하는 데이터테이블로 설계된 component와 Action 클래스들

객체지향에서 클래스를 설계할 때, 부모-자식 관계의 상속(Inheritance)이 너무 깊어지면 코드가 경직되고 복잡해집니다. 이를 해결하기 위해 기능들을 '컴포넌트(Component)' 단위로 조립(Composition)하는 방식이 훨씬 유연하다고 생각하여 설계하였습니다.


1) Skillcomponent
SkillComponent는 스킬 사용 흐름을 제어하는 컨트롤러 역할을 담당합니다.

-마나 소모
-쿨타임 검증
-몽타주 실행
-서버 권한 검증

공통 처리만 담당하며,
스킬의 실제 실행 로직은 SkillAction 클래스로 분리했습니다.

이를 통해 SkillComponent는 복잡한 분기 없이
모든 스킬을 동일한 실행 흐름으로 처리할 수 있습니다.

#### Skill Execution Flow

SkillComponent의 스킬 사용은 서버 권한(Server RPC)에서 처리됩니다.
`server_SkillAction_Implementation` 함수는 스킬 실행의 공용 진입점으로,
플레이어와 몬스터의 스킬 로직을 하나의 흐름으로 관리합니다.

처리 흐름은 다음과 같습니다:
- 스킬 사용에 가능 여부 검증 (쿨타임, 슬롯, 마나)
- 테이블 기반 스킬 데이터 조회
- 플레이어 / 몬스터 분기 처리
- SkillAction 클래스 기반 실행
- 애니메이션 몽타주 재생 및 캐스팅 상태 관리

이 구조를 통해 신규 스킬 추가 시
C++ 수정 없이 DataTable + SkillAction 클래스 추가만으로 확장이 가능합니다.

void USkillComponent::server_SkillAction_Implementation(FName SkillTID)
{
    FSkillData* SkillData = IsUsedSkill(SkillTID);
    if (!SkillData) return;

    FSkillTableRow* SkillTable =
        TableSystem->FindTableRow<FSkillTableRow>(TABLE_NAME::SKILL, SkillTID);

    if (!SkillTable) return;

    OwnerCharacter->SetIsCastingCheck(true);
    OwnerCharacter->MontageAction(
        OwnerCharacter->GetSkillMontage(SkillTable->MontageID), true);
}

2) SkillActionBase

스킬의 실제 동작 로직은
SkillActionBase를 상속받는 개별 Action 클래스로 구현했습니다.

-Projectile
-Area
-Buff / Debuff 등

각 스킬 타입은 독립적인 클래스로 구성되어 있으며,
신규 스킬 타입 추가 시 기존 코드 수정 없이 확장이 가능합니다.

모든 스킬 판정 및 데미지 계산은
서버 권한에서만 처리되어 멀티플레이 환경에서도 일관성을 유지합니다.

bool UAreaAction::SkillAction(ACC_CharacterBase* Owner)
{
    if (!Owner || !Owner->HasAuthority())
        return false;

    const FSkillTableRow SkillTable = Owner->GetCurrentSkillData();
    const FAreaActionData& AreaData = SkillTable.AreaActionData;

    // 쿨타임 및 마나 처리
    Owner->GetSkillComp()->CurrentSkillResetCoolDown();
    Owner->ApplyMP(-SkillTable.CostMana);

    // 범위 판정 (콘형 / 선형)
    TArray<ACC_CharacterBase*> Targets =
        FindAreaTargets(Owner, AreaData);

    // 데미지 계산 및 적용
    for (ACC_CharacterBase* Target : Targets)
    {
        float Damage = CalculateFinalDamage(Owner, Target, SkillTable);
        UGameplayStatics::ApplyDamage(Target, Damage,
            Owner->GetController(), Owner, UDamageType::StaticClass());
    }

    return Targets.Num() > 0;
}

3) AbilityComponent
AbilityComponent는 캐릭터의 모든 능력치를 중앙에서 관리하는 컨테이너입니다.

기본 스탯, 아이템, 버프/디버프 등
여러 시스템에서 능력치가 변경되더라도
기본 스탯을 직접 수정하지 않도록 구조를 분리했습니다.

모든 능력치 변경은 RefreshTotalAbil()을 통해 단일 경로로 계산되며,
UI는 최종 결과만 전달받도록 설계했습니다.

- 능력치는 카테고리 단위로 분리되어 관리됩니다.

분류            	설명
DefaultAbils	캐릭터 기본 능력치
ItemEquipAbils	장착 아이템 효과
OwnedAbils	소지 아이템 효과
BuffAbils	        버프 효과 (시간 제한)
DeBuffAbils	디버프 효과 (시간 제한)
TotalAbils     	최종 합산 결과

각 카테고리는 독립적으로 추가/삭제 가능하며
RefreshTotalAbil()에서 단일 계산 루트로 합산됩니다.

4) Inventory & Equipment Component
InventoryComponent : 아이템 스택 여부 및 최대 스택 수를 데이터 테이블을 기준으로 자동 제어하며, 
멀티플레이 동기화를 위해 정렬 기능은 서버에서 수행됩니다.

EquipmentComponent : 아이템 장착 시 UI 반영은 물론
AbilityComponent의 Equip 컨테이너에 능력치가 즉각 연동되도록 설계하여 장비와 능력치 시스템 간의 결합도를 낮췄습니다.

5) SpawnManagerComponent
몬스터 및 NPC 스폰을 데이터 테이블 기반으로 제어합니다. 스폰 좌표, 타입, 조건 등을 테이블화하여
 UActiveSpawnPoint 인스턴스로 관리하며, 코드 수정 없이 새로운 스폰 패턴을 확장할 수 있습니다.

---

## 3. Animation Notify System

1) AnimNotify_SkillAction
스킬 사용 시 반드시 몽타주가 실행되는 흐름을 전제로,
모든 스킬 로직은 단일 AnimNotify를 통해 처리하도록 설계했습니다.

`AnimNotify_SkillAction`은
스킬 종류와 무관하게 공통으로 사용되는 노티파이로,
몽타주에 해당 노티파이 하나만 배치하면
데이터 테이블에 정의된 정보에 따라
스킬 로직이 자동으로 실행되도록 구성했습니다.

이를 통해 애니메이터나 다른 팀원이
스킬별 노티파이를 개별적으로 관리할 필요 없이,
일관된 방식으로 스킬을 연결할 수 있도록 했습니다.

또한 `IsLocallyControlled()` 체크를 통해
실제 스킬을 사용한 컨트롤러만 로직을 실행하도록 방어 로직을 추가하여,
멀티플레이 환경에서 발생할 수 있는 예외 상황을 고려했습니다.

void UAnimNotify_SkillAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

	ACC_CharacterBase* character = Cast<ACC_CharacterBase>(MeshComp->GetOwner());
	if (character)
	{
        USkillComponent* SkillComp = character->GetSkillComp();
        if (SkillComp)
        {
            SkillComp->EraseSkillIndicator();

            if(character->IsLocallyControlled())
                SkillComp->Server_ExecuteSkillAction();
        }
    }
}

2) AnimNotify_MonsterDead

몬스터 사망 처리 역시 단일 AnimNotify로 통합하여 설계했습니다.

`AnimNotify_MonsterDead`는
몬스터 종류와 관계없이 공통으로 사용되며,
몬스터가 보유한 아이템 드롭 정보와 확률은
에디터에서 설정 가능한 데이터로 분리했습니다.

이를 통해 몬스터 사망 시
아이템 드롭, 연출 처리, 자연스러운 소멸까지의 흐름을
하나의 노티파이로 일관되게 처리할 수 있으며,
디자이너가 코드 수정 없이
드롭 테이블을 조정할 수 있도록 구성했습니다.

void UAnimNotify_MonsterDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(MeshComp->GetOwner());
	
	if (Monster)
		Monster->OnDeathAnimationFinished();
}


---

## 4. GameModeBase/GameStateBase
GameModeBase는 레벨 단위의 전투 흐름을 제어하며,
실제 상태 데이터는 GameStateBase를 통해 관리됩니다.

이를 통해 멀티플레이 환경에서
모든 플레이어가 동일한 전투 상태를 공유할 수 있도록 구성했습니다


## Why This Architecture Matters

유지보수 비용 감소
→ 콘텐츠 추가 시 코드 수정 없이 데이터만 변경

협업 효율 증가
→ 기획자, UI 개발자가 내부 로직을 몰라도 작업 가능

멀티플레이 안정성 확보
→ 모든 핵심 판정 로직을 서버 권한 기반으로 처리

