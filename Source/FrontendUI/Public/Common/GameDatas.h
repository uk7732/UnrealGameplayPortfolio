// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameDefines.h"
#include "BehaviorTree/BehaviorTree.h"

#include "GameDatas.generated.h"
/**
 *
 */

 // --- 스킬(Skill) 시스템 관련 데이터 ---
USTRUCT(BlueprintType)
struct FSkillData
{
    GENERATED_BODY()

    FSkillData() : TID(NAME_None), CoolDown(0.f), CurrentCoolDown(0.f), CostMana(0.f) {}
    FSkillData(FName& tid, float cooldown, float costmana) : TID(tid), CoolDown(cooldown), CostMana(costmana) {}
    FSkillData(FName& tid, float cooldown, float costmana, float Range) : TID(tid), CoolDown(cooldown), CostMana(costmana), SkillRange(Range) {}

    // 스킬 데이터 테이블의 행(Row) 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName TID;

    // 전체 쿨타임
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CoolDown = 0;

    // 현재 남은 쿨타임 (이 값이 0이 되어야 스킬 사용 가능)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentCoolDown = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CostMana = 0;  //소모 마나 값

    //스킬사거리 몬스터 AI에 적용하기 위해 추가함
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float SkillRange = 0;

    // TArray 검색을 위한 연산자
    bool operator==(const FName InTID) const
    {
        return this->TID == InTID;
    }
};


// 스킬의 대미지 계산에 어떤 능력치를 얼마만큼 반영할지 정의하는 구조체
USTRUCT(BlueprintType)
struct FSkillRateData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eAbilType Type;

    // 계수 (예: 공격력의 1.5배 -> Rate = 1.5f) 
    // Defence 지정시 숫자가 방어력 관통 수치 ex Rate = 5 면 방어력 5프로 감소 후 데미지계산 로직
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Rate = 0;
};

USTRUCT(BlueprintType)
struct FWidgetActivatableData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag WidgetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<class UWidget_InGameActivatableWidget> Widget_Activatable;
};




USTRUCT(BlueprintType)
struct FMapData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MapTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MapNumber = -1;
    
    // UI에 표시될 맵 이름 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    // 맵 설명 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // 맵 썸네일 이미지 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> MapImage;

    bool operator==(const int32 Number) const
    {
        return this->MapNumber == Number;
    }

    bool operator<(const FMapData& Other) const
    {
        return MapNumber < Other.MapNumber;
    }
};

USTRUCT(BlueprintType)
struct FMapGroupData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MapNumber = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MapTID;
};


USTRUCT(BlueprintType)
struct FRoomData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RoomNum = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MonsterCount = 0;

    FRoomData() {}
    FRoomData(int32 InNum, int32 InCount) : RoomNum(InNum), MonsterCount(InCount) {}
};




USTRUCT(BlueprintType)
struct FSkillSlotData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotNum = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillTID;

};

USTRUCT(BlueprintType)
struct FMontageMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FName MontageID; //몽타주를 식별하는 키(테이블키 아님)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* MontageAsset = nullptr; //실제 몽타주 파일 폴더

    friend bool operator==(const FMontageMapping& A, FName B)
    {
        return A.MontageID == B;
    }

};

USTRUCT(BlueprintType)
struct FAreaActionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)//부채꼴 중심선에서 양쪽으로 벌어지는 각도 (45도면 90도 부채꼴) 0도면 선형타격 0도이상 콘모양 타격임
        float ConeHalfAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 0.0f;  //Area 직선 영역 타격시 폭넓이, 원뿔 영역 타격시 사용안함 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinDistance = 0.0f;    //최소거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxDistance = 0.0f;    //최대거리

};

USTRUCT(BlueprintType)
struct FBuffActionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillRateData> Rates;  //스킬에 사용되는 다양한 비율, 계수 목록 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AddAttackStat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AddDefenceStat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f; // 지속시간 
};


USTRUCT(BlueprintType)
struct FDebuffActionData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillRateData> Rates;  //스킬에 사용되는 다양한 비율, 계수 목록 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinusAttackStat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinusDefenceStat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f; // 지속시간
};


USTRUCT(BlueprintType)
struct FProjectileBehaviorData
{
    GENERATED_BODY()

    // 1. 행동 타입 (필수)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eProjectileFinalAction ActionType = eProjectileFinalAction::INSTANTLY_DESTROY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AProjectileBase> ProjectileClass;

    //============2. 발사 위치 관련 ==============
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Forward = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RIght = 0.f; //좌측으로 이동시 음수값 넣으면 됨

    //============ 3. 폭발 관련 =======================
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExplosionRadius = 0.f;            // 폭발 최대 반경

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExplosionDamageRatio = 1.0f;      // 광역 피해 계수 (1.0 = 100%)

    float ExplosionImpulse = 0.f;
    //=================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f; // 지속시간
};

USTRUCT(BlueprintType)
struct FRangedEffectBehaviorData
{
    GENERATED_BODY()

    //장판스킬 Actor 지정
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ARangedEffectBase> RangedEffect;

    //즉발피해 스킬 시 이팩트가 몇초동안 남아있을지 결정
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float InstantDestroyDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectRadius = 0.0f; // 기본값 설정

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationTickDamage = 0.0f; //지속데미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f; // 지속시간
};

// 능력치 데이터 하나를 표현하는 구조체
USTRUCT(BlueprintType)
struct FAbilityData
{
    GENERATED_BODY()

    FAbilityData() {}
    FAbilityData(const FAbilityData& InData) : Type(InData.Type), ModifierType(InData.ModifierType), DurationTime(InData.DurationTime), TID(InData.TID), Value(InData.Value) {}
    FAbilityData(eAbilType InType, eAbilModifierType mft, float Duration, float InValue) : Type(InType), ModifierType(mft), DurationTime(Duration), Value(InValue) {}
    FAbilityData(eAbilType InType, float InValue) : Type(InType), Value(InValue) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eAbilType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eAbilModifierType ModifierType;

    //버프, 디버프스킬 남은 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationTime = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value;

    bool operator==(const eAbilType InType) const
    {
        return this->Type == InType;
    }
};

USTRUCT(BlueprintType)
struct FItemDropData
{
    GENERATED_BODY()

    //0~100까지 숫자 넣으면 됨 아이템 드롭 확률 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ItemDropchance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ITemTID;

    //컴포넌트 , 컨슘 아이템일시 최대 몇개까지 떨어트릴 수 있게 할건지 지정
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxDropCount = 1;
};


USTRUCT(BlueprintType)
struct FSpawnZoneData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName CharacterTID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName SpawnName = FName();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Location;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FRotator Rotation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 RoomNumber = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float SpawnRadiusOrSide = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float MaxSpawnCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentSpawnCount = 0;

    //IntervalTime은 꼭 5 단위로 작성해 주세요 . (Timer를 5초단위로 확인하게 세팅해둠)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float SpawnInterval = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentIntervalTime = 0;
};



USTRUCT(BlueprintType)
struct FNPCTIDDatas
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MapGroupTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ITemShopTID;
};


USTRUCT(BlueprintType)
struct FProjectileSpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AProjectileBase> CasterClass; // 투사체 클래스

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Delay; // 발사 딜레이

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count; // 발사 갯수
};

USTRUCT(BlueprintType)
struct FSenseConfigData
{
    GENERATED_BODY()
    //인식 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SightRadius = 0.0f;
    //놓치는 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LoseSightRadius = 0.0f;
    //시야각
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PeripheralVisionAngleDegrees = 0.0f;
    //공격사거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange = 0.0f;
    //패트롤(정찰) 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PatrolRadius = 0;
    //패트롤(정찰) 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CombatSpeed = 0;
    //패트롤(정찰) 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float IdleSpeed = 0;
};


//드롭 확률 구조체
USTRUCT(BlueprintType)
struct FDropProbData
{
    GENERATED_BODY()

    FDropProbData() {}
    ~FDropProbData() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Prob;  //드롭 확률

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RewardTID;  //보상 정의 테이블 TID
};

//보상 정의 구조체
USTRUCT(BlueprintType)
struct FRewardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eRewardType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Count;
};

//코인 타입 구조체
USTRUCT(BlueprintType)
struct FCoinData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eCoinType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count;

    FCoinData& CopyFrom(FCoinData& other)
    {
        TID = other.TID;
        Type = other.Type;
        Count = other.Count;

        return *this;
    }

    FCoinData& CopyFrom(FName& tid, eCoinType type, int32 count)
    {
        TID = tid;
        Type = type;
        Count = count;

        return *this;
    }
};

USTRUCT(BlueprintType)
struct FMissionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestTID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eProgressType ProgressType;

    bool operator == (const FName tid)
    {
        return this->TID == tid;
    }

    void CopyFrom(FMissionData& other)
    {
        QuestTID = other.QuestTID;
        TID = other.TID;
        Value = other.Value;
        ProgressType = other.ProgressType;
    }

    void CopyFrom(const FMissionData& other)
    {
        QuestTID = other.QuestTID;
        TID = other.TID;
        Value = other.Value;
        ProgressType = other.ProgressType;
    }
};

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 UID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Enchant = 0;

    FItemData& CopyFrom(FItemData& other)
    {
        UID = other.UID;
        TID = other.TID;
        Count = other.Count;
        Enchant = other.Enchant;

        return *this;
    }

    FItemData& CopyFrom(int64 uid, FName& tid, int32 count, int32 enchant)
    {
        UID = uid;
        TID = tid;
        Count = count;
        enchant = Enchant;

        return *this;
    }

    bool operator==(const int64 InType) const
    {
        return this->UID == InType;
    }
};


USTRUCT(BlueprintType)
struct FItemEquipData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eItemEquipType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 UID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Enchant;

    FItemEquipData& CopyFrom(FItemEquipData& other)
    {
        Type = other.Type;
        UID = other.UID;
        TID = other.TID;
        Enchant = other.Enchant;

        return *this;
    }

    bool operator==(const eItemEquipType InType) const
    {
        return this->Type == InType;
    }

};

USTRUCT(BlueprintType)
struct FItemEquipSlotData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    eItemEquipType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FItemEquipData ItemEquipData;

    FItemEquipSlotData& CopyFrom(FItemEquipSlotData& other)
    {
        Type = other.Type;
        ItemEquipData = other.ItemEquipData;
    }
    bool operator==(const eItemEquipType InType) const
    {
        return this->Type == InType;
    }
};

UCLASS()
class FRONTENDUI_API UGameDatas : public UObject
{
    GENERATED_BODY()

};
