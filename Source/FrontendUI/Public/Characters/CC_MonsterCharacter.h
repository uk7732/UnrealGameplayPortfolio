// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/CC_CharacterBase.h"
#include "Spawn/ActiveSpawnPoint.h"
#include "UObject/NoExportTypes.h"
#include "CC_MonsterCharacter.generated.h"


//
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackAnimationEnded, bool);
/**
 *
 */
UCLASS()
class FRONTENDUI_API ACC_MonsterCharacter : public ACC_CharacterBase
{
    GENERATED_BODY()

public:
    ACC_MonsterCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MonsterTID)
    FName MonsterTID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
    int32 SpawnRoomNum = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
    float MoveSpeed;

    //아이템 드롭 리스트 Blueprint 에서 작성하기
    UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Item")
    TArray<FItemDropData> ItemDropList;

    UPROPERTY(EditAnywhere, Category = "AI")
    class UBehaviorTree* BTAsset;

    //인식거리 놓치는거리 시야각 설정
    UPROPERTY(EditAnywhere, Category = "AI")
    FSenseConfigData SenseConfigData;

    FTimerHandle SetRotationHandle;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    UFUNCTION()
    void SetSpawnRoomNum(int32 num);
    UFUNCTION()
    int32 GetSpawnRoomNum();
    //스폰포인트클래스의 몬스터 객체수 감소 체크를 위한 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMonsterDeath OnMonsterDeath;
    //델리게이트 호출 함수
    void Die();
    //테스크 초기화를 위한 델리게이트
    FOnAttackAnimationEnded OnAttackAnimationEnded;

    void SetMonsterData(const FName& monsterTID);

    UFUNCTION(BlueprintCallable)
    float GetMoveSpeed();

    // 체력바 띄우는 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float HPBarVisibleDistance = 10000.0f;

    ACC_CharacterBase* GetAggroTarget();
    // ACharacterBase의 함수를 오버라이드하여 몬스터에 맞게 구현합니다.

    void SetRotationUpdate();
    UFUNCTION()
    void OnRep_MonsterTID();

    class UBehaviorTree* GetBehaviorTree();
    FSenseConfigData GetSenseConfigData();

    void OnDeathAnimationFinished();
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    virtual float TakeDamage(float DamageAmount, AActor* DamageCauser) override;

    virtual void HandleAttackAnimationEnded(bool bInterruped) override;
private:
    void ItemDrop();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
