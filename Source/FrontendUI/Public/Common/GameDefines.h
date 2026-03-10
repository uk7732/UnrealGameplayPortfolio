// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameDefines.generated.h"



//애니메이션 타입 열거형
UENUM(BlueprintType)
enum class eAnimType : uint8
{
    ANIM_TYPE_IDLE UMETA(DisplayName = "Idle"),
    ANIM_TYPE_WALK UMETA(DisplayName = "Walk"),
    ANIM_TYPE_SPRINT UMETA(DisplayName = "Sprint"),
    ANIM_TYPE_JUMP UMETA(DisplayNAme = "Jump"),
    ANIM_TYPE_JUMPEND UMETA(DisplayNAme = "JumpEND"),
    ANIM_TYPE_ATTACK UMETA(DisplayName = "Attack"),
    ANIM_TYPE_RELOAD UMETA(DisplayName = "Reload"),
    ANIM_TYPE_HIT UMETA(DisplayName = "Hit"),
    ANIM_TYPE_DEATH UMETA(DisplayName = "Death"),
};

//애니메이션 타입 열거형
UENUM(BlueprintType)
enum class eDecalType : uint8
{
    DECAL_NONE UMETA(DisplayName = "DECAL_NONE"),
    DECAL_CIRCLE UMETA(DisplayName = "DECAL_CIRCLE"),
    DECAL_BOX UMETA(DisplayName = "DECAL_BOX"),
    DECAL_CONE UMETA(DisplayName = "DECAL_CONE"),
};

// 능력치(Ability) 관련 데이터
// 능력치 종류를 정의하는 열거형
UENUM(BlueprintType)
enum class eAbilType : uint8
{
    ABIL_TYPE_NONE UMETA(DisplayName = "None"),
    ABIL_TYPE_ATTACK UMETA(DisplayName = "Attack"),
    ABIL_TYPE_DEFENCE UMETA(DisplayName = "Defence"), 
    ABIL_TYPE_MAX_HP UMETA(DisplayName = "Max_HP"),
    ABIL_TYPE_MAX_MP UMETA(DisplayName = "Max_MP"),     
    ABIL_TYPE_MAX_MP_PS UMETA(DisplayName = "MP_PS"), // 초당 마나회복량
};


UENUM(BlueprintType)
enum class eAbilCategoryType : uint8
{
    ABIL_CATEGORY_DEFAULT UMETA(DisplayName = "Default"),
    ABIL_CATEGORY_ITEM_EQUIP UMETA(DisplayName = "Item Equip"),
    ABIL_CATEGORY_ITEM_OWNED UMETA(DisplayName = "Item Owned"),
    ABIL_CATEGORY_BUFF UMETA(DisplayName = "Buff"),
    ABIL_CATEGORY_DEBUFF UMETA(DisplayName = "DeBuff"),
    ABIL_CATEGORY_TOTAL UMETA(DisplayName = "Total"),
};

UENUM(BlueprintType)
enum class eAbilModifierType : uint8
{
    ABIL_MODIFIER_ADD UMETA(DisplayName = "Add"),
	ABIL_MODIFIER_MINUS UMETA(DisplayName = "Minus"),
    ABIL_MODIFIER_PERCENT UMETA(DisPlayNname = "Percent"),
};

UENUM(BlueprintType)
enum class eSpawnCharacterType : uint8
{
    SPAWN_NONE UMETA(DisplayName = "Spawn_None"),
    SPAWN_MONSTER UMETA(DisplayName = "Spawn_Monster"),
    SPAWN_NPC UMETA(DisplayName = "Spawn_NPC"),
};

UENUM(BlueprintType)
enum class eNPCType : uint8
{
    NPC_MAP UMETA(DisplayName = "NPC_Map"),
    NPC_ITEM UMETA(DisplayName = "NPC_Item"),
};

UENUM(BlueprintType)
enum class eConditionType : uint8
{
    STAGE_LEVEL UMETA(DisplayNAme = "STAGE_LEVEL"),
    ABILITY_VALUE UMETA(DisplayName = "ABILITY_OPEN"),
    PLAYER_LEVEL UMETA(DisplayName = "ITEM_EQUIP"),
    MONSTER_KILL  UMETA(DisplayName = "MONSTER_KILL"),
};



UENUM(BlueprintType)
enum class eRewardType : uint8
{
    Coin	UMETA(DisplayName = "Coin"),
    Item	UMETA(DisplayName = "Item"),
};

UENUM(BlueprintType)
enum class eCoinType : uint8
{
    GOLD	UMETA(DisplayName = "GOLD"),
    SILVER	UMETA(DisplayName = "SILVER"),
    RUBY	UMETA(DisplayName = "RUBY"),
};


UENUM(BlueprintType)
enum class eItemType : uint8
{
    WEAPON_R	UMETA(DisplayName = "Weapon_R"),
    WEAPON_L	UMETA(DisplayName = "Weapon_L"),
    SHIELD  UMETA(DisplayNema = "SHIELD"),
    DEFENCE	UMETA(DisplayName = "ARMOR"),
    HELMET  UMETA(DisplayName = "HELMET"),
    CONSUME	UMETA(DisplayName = "CONSUME"),      //소모품 
    COMPONENT UMETA(DisplayName = "COMPONENT"),  //부품
};


UENUM(BlueprintType)
enum class eItemEquipType : uint8
{
    NONE        UMETA(DisplayName = "NONE"),
    WEAPON_R	UMETA(DisplayName = "WEAPON_R"),
    WEAPON_L UMETA(DisplayName = "WEAPON_L"),
    SHIELD	  UMETA(DisplayName = "SHIELD"),
    ARMOR	 UMETA(DisplayName = "ARMOR"),
    HELMET  UMETA(DisplayName = "HELMET"),
};

UENUM(BlueprintType)
enum class eSkillType : uint8
{
    SKILL_TYPE_NORMAL_ATTACK UMETA(DisplayName = "Skill_NomalAttack"),  //기본공격 (좌클릭)
    SKILL_TYPE_SUB_NORMAL_ATTACK UMETA(DisplayName = "Skill_SubNomalAttack"),  //기본공격 (우클릭)
    SKILL_TYPE_AREA UMETA(DisplayName = "Skill_Area"),                  //영역, 경로기반 공격스킬(즉발 직선이나 원뿔모양으로 근접공격이나 관통스킬 등)
    SKILL_TYPE_RANGED UMETA(DisplayName = "Skill_Ranged"),              //원거리 스킬
    SKILL_TYPE_PROJECTILE UMETA(DisplayName = "Skill_Projectile"),      //발사체 스킬
    SKILL_TYPE_BUFF UMETA(DisplayName = "Skill_Buff"),                  //버프
    SKILL_TYPE_DEBUFF UMETA(DisplayName = "Skill_Debuff"),              //디버프
};

UENUM(BlueprintType)
enum class eProjectileFinalAction : uint8
{
    INSTANTLY_DESTROY UMETA(DisplayName = "Instantly_Destroy"),     //대상에게 피해를 입히고 즉시 사라짐(총알/화살)
    EXPLODE_ONHIT      UMETA(DisplayName  = "Explode_Onhit"),       //대상에게 피해를 입히고 지정된 반경에 추가 광역피해 후 사라짐
    TIMED_EXPLOSION    UMETA(DisplayName = "Timer_explosion"),      //충돌 여부와 상관없이 Duration이 끝나면 지정된 위치에서 폭발 광역피해(설치형 폭탄, 수류탄)
};

UENUM(BlueprintType)
enum class eQuestType : uint8
{
    QUEST_TYPE_NORMAL UMETA(DisplayName = "QUEST_TYPE_NORMAL"),  
    QUEST_TYPE_CONNECT UMETA(DisplayName = "QUEST_TYPE_CONNECT"),
    QUEST_TYPE_END UMETA(DisplayName = "QUEST_TYPE_END")
};

UENUM(BlueprintType)
enum class eProgressType : uint8
{
    PROGRESS_TYPE_READY	UMETA(DisplayName = "PROGRESS_TYPE_READY"),
    PROGRESS_TYPE_PRGRESS UMETA(DisplayName = "PROGRESS_TYPE_PRGRESS"),
    PROGRESS_TYPE_COMPLETE UMETA(DisplayName = "PROGRESS_TYPE_COMPLETE"),
    PROGRESS_TYPE_REWARDED UMETA(DisplayName = "PROGRESS_TYPE_REWARDED"),
};

UENUM(BlueprintType)
enum class eSpawnModeType : uint8
{
    NONE UMETA(DisplayName = "None"),
    SINGLE_POINT UMETA(DisplayName = "SinglePoint"),
    RANDOM_CIRCLE UMETA(DisplayName = "RandomCircle"),
    RANDOM_RECTANGLE UMETA(DisplayName = "RandomRectangle"),
};

UENUM(BlueprintType)
enum class ePacketResult : uint8
{
    SUCCESS	UMETA(DisplayName = "PROGRESS_TYPE_READY"),
    INVALID UMETA(DisplayName = "PROGRESS_TYPE_PRGRESS"),
    FAIL UMETA(DisplayName = "PROGRESS_TYPE_COMPLETE"),
    FATAL UMETA(DisplayName = "PROGRESS_TYPE_REWARDED"),
};





UCLASS()
class FRONTENDUI_API UGameDefines : public UObject
{
	GENERATED_BODY()
	
};
