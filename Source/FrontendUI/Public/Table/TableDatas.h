// Vince Petrelli All Rights Reserved
//skill , player CSV는 추가 후 다시 만들것
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h" // FTableRowBase를 사용하기 위해 필요합니다.
#include "Common/GameDefines.h"
#include "Common/GameDatas.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "BehaviorTree/BlackboardData.h"
#include "Actor/Item/ItemBase.h"
#include "Animation/AnimBase.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Characters/CC_MonsterCharacter.h"
#include "TableDatas.generated.h"


#define TABLE_NAME UTableDatas::TableName
#define DEFINE_NAME UTableDatas::Define

// 데이터 테이블들을 로드하기 위한 경로 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FTableLoadRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TablePath; // 로드할 데이터 테이블의 경로

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsLoad; // 이 테이블을 로드할지 여부
};

USTRUCT(BlueprintType)
struct FSpawnGroupTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SpawnTableTIDs;
};

USTRUCT(BlueprintType)
struct FNPCTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eNPCType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACC_NPCCharacter> NPCClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name; // NPC 이름

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DialogueID; // 대화 스크립트 ID

    // 이 NPC에게 적용할 AI (비헤이비어 트리 에셋)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UBehaviorTree> AIBehavior;

    // 맵 GroupTID(맵리스트), Item 상점 등 TID 모아둔 구조체 NPCTyp 이 늘어나면 이 구조체에 추가하여 샤용하면 됨
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNPCTIDDatas NPCTIDDatas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TagTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnimationTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FWidgetActivatableData> WidgetEntry;

    //처음 눌렀을때 띄울 Widget Tag 입력하기
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag StartWidgetTag;
};


USTRUCT(BlueprintType)
struct FSpawnTableRow : public FTableRowBase
{
    //NPC는 SINGLE_POINT 로 웬만하면 통일하기
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eSpawnModeType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eSpawnCharacterType CharacterType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SpawnName;
    // 스폰할 몬스터의 DT_아이디
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MonsterTID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector SpawnLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator SpawnRotation;

    //원의 지름, 사각형의 한 변의 길이 Type이 SinglePoint일시 지정안함
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnRadiusOrSide = 0;;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnCount = 0;

    //스폰할 룸넘버 입력 상호작용 하지 않을시 그냥 0으로 둬도 됨
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnRoomNumber = 0;

    //꼭 5단위로 작성해 주세요 (5초 주기 확인이기 떄문)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnInterval = 0;

    //true 로 둘 경우 초기 값(SpawnInterval)을 넣어도 스폰되지 않음 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsTriggerSpawn = false;
};

// 게임 내 전역 상수를 정의하기 위한 구조체
USTRUCT(BlueprintType)
struct FDefineTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int IntVal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatVal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString StrVal;
};

USTRUCT(BlueprintType)
struct FPlayerTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACC_PlayerCharacter> PlayerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TagTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilGroupTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SkillIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TeamID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnimationTID;
};

// TableDatas.h
USTRUCT(BlueprintType)
struct FMonsterTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // 스폰할 몬스터의 C++ 클래스 (또는 블루프린트)
    // TSubclassOf는 특정 클래스를 상속받는 클래스 타입만 지정할 수 있게 해줍니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACC_MonsterCharacter> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MonsterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TagTID;

    // 이 몬스터가 사용할 능력치 그룹 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilGroupTID;

    // 이 몬스터가 보유할 스킬 ID 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SkillIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnimationTID;

};

USTRUCT(BlueprintType)
struct FAITableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBehaviorTree* Tree;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBlackboardData* Board;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SightRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LoseSightRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PeripheralVisionAngleDegrees;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDetectEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDetectFriendlies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDetectNeutrals;
};

// 스킬 데이터 테이블의 행 구조체
USTRUCT(BlueprintType)
struct FSkillTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eSkillType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class USkillActionBase> SkillActionClass;

    //근접공격(특정 범위 즉발타격)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FAreaActionData AreaActionData;

    //투사체 관련 핵심 데이터 정리(필요데이터들)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FProjectileBehaviorData ProjectileBehaviorData;  
    
    //장판스킬 Actor 지정(Ranged스킬)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRangedEffectBehaviorData RangedEffectBehaviorData;
    
    //==============Buff, Debuff 는 공용 Rates, Damage를 사용하지 않음 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBuffActionData BuffActionData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDebuffActionData DebuffActionData;
    //=====================================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CoolDown = 0.0f; // 쿨타임

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CostMana = 0.0f; //소모 마나량

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillRateData> Rates;  //스킬에 사용되는 다양한 비율, 계수 목록 (Typa Attack = 공격력 증가 / Type == Defence 방어력 관통)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0;    // 스킬이 단일타겟에게 입히는 기본적인 피해 수치 최종피해량은 Rates가 곱해져서 결정될 수 있음

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MontageID;  //테이블에서 해당 스킬 아이디만 지정 사용할 스킬 이름으로 통일하기

    // UI에 표시할 스킬 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* SkillIcon;
};


USTRUCT(BlueprintType)
struct FAnimationTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UAnimBase> Animation;
};


// 능력치 그룹 데이터 테이블의 행 구조체
USTRUCT(BlueprintType)
struct FAbilGroupTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // 능력치 데이터의 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAbilityData> Abils;
};

USTRUCT(BlueprintType)
struct FAbilTableRow : public FTableRowBase
{
    GENERATED_BODY()
  
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eAbilType Type;
    
    //조건체크 모아둔TID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> ConditionTIDs;
};

//enum 값을 TID 로 저장하기 위해 만들어진 TableRow
USTRUCT(BlueprintType)
struct FAbilCategoryRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eAbilCategoryType Type;
};


//todo : map path 도 TSoftObjectPtr<UWorld> 로 바꾸거나 Spawn TID(선생님이 한것처럼 ) 구현할지 고민
USTRUCT(BlueprintType)
struct FMapTableRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MapPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PlayerStartLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator PlayerStartRotation;

    // UI에 표시될 맵 이름 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    // 맵 설명 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // 맵 썸네일 이미지 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> MapImage;
};

USTRUCT(BlueprintType)
struct FMapGroupTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FMapGroupData> MapGroupList;
};

USTRUCT(BlueprintType)
struct FTagTableRow : public FTableRowBase
{
    GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Tags;
};

//enum 값으로 빼서 관리할지 TID 로 빼서 관리할지 고민
USTRUCT(BlueprintType)
struct FTeamTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TeamSrc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TeamDest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Attitude;
};


USTRUCT(BlueprintType)
struct FRewardTableRow : public FTableRowBase
{
    GENERATED_BODY()

    //eRewardType Type : 보상의 타입
    //아이템, 코인
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eRewardType Type;

    //코인이면 --> 코인테이블 ID
    //아이템이면 --> 아이템 테이블 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RewardID;

    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Count;
};

USTRUCT(BlueprintType)
struct FCoinTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eCoinType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxCount;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // 아이템 종류
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eItemType Type;

    // 바닥에 버려진 상태에서 생성된 액터 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AItemBase> ItemClass;

    // 적용할 능력치 그룹ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilGroupTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipTID;
    
    // 아이템 메시
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* Mesh;

    // UI에서 보여질 이미지텍스쳐
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Image;

    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

    // 아이템에 대한 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemDesc;

    // 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EnchantTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TagTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnimationTID;

    // 최대 스택 수 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStackCount = 0;
};

USTRUCT(BlueprintType)
struct FItemEquipTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eItemEquipType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ConditionTID;
};

USTRUCT(BlueprintType)
struct FItemEnchantTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRewardData> ConsumeDatas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Prob;    //확률
};

USTRUCT(BlueprintType)
struct FConditionTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eConditionType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Param1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Param2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Value;
};



// UTableDatas 클래스 자체는 데이터를 담기보다,
// 데이터 테이블의 이름(FName)과 같은 상수들을 모아두는 역할을 합니다.
UCLASS()
class FRONTENDUI_API UTableDatas : public UObject
{
	GENERATED_BODY()

public:
    // 데이터 테이블의 이름을 상수로 관리하기 위한 클래스
    class TableName
    {
    public:
        static const FName MAP;
        static const FName MAPGROUP;
        static const FName MONSTER;
		static const FName SPAWN;
        static const FName SPAWNGROUP;
        static const FName DEFINE;
        static const FName ABILGROUP;
        static const FName AI;
        static const FName ABIL;
		static const FName ABILCATEGORY;
        static const FName PLAYER;
        static const FName SKILL;
		static const FName REWARD;
		static const FName COIN;
		static const FName ITEM;
		static const FName ITEMEQUIP;
		static const FName ITEMENCHANT;
		static const FName TAG;
        static const FName TEAM;
		static const FName ANIMATION;
        static const FName CONDITION;
        static const FName NPC;

        // ... (원본 코드의 나머지 테이블 이름들) ...
    };

    // Define 테이블 내부의 특정 행 이름을 상수로 관리하기 위한 클래스
    class Define
    {
    public:
        static const FName MaxProb;
        static const FName Player_DefaultTID;
        static const FName SprintStart_RotationRate;
        static const FName SprintStop_RotationRate;
        static const FName SprintSpeed;
        static const FName WalkSpeed;
        static const FName SpeedInterpRate;
        static const FName MonsterSpawn_01_TID;

        static const FName LobbySpawn;

        // ... (원본 코드의 나머지 Define 이름들) ...
    };
};
