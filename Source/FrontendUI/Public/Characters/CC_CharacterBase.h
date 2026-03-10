// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Common/GameDatas.h"
#include "Components/WidgetComponent.h"

#include "CC_CharacterBase.generated.h"

class UAbilityComponent;
class USkillComponent;
struct FPlayerTableRow;
class SpringArmComponent;
class UCameraComponent;
class UWidget_FloatingDamage; 

UCLASS()
class FRONTENDUI_API ACC_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACC_CharacterBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<class UAnimBase> Anim;

	// AbilityComponent를 담을 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Abilities")
	TObjectPtr<UAbilityComponent> Ability;

	// SkillComponent를 담을 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Combat")
	TObjectPtr<USkillComponent> Skill;

	// 머리 위 HP 바 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HPBarWidget;

	// 클라이언트에게 데미지 텍스트를 띄우라고 명령하는 함수
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ShowDamageText(float DamageAmount, bool bIsCritical, AActor* DamagedActor);
	void Multicast_ShowDamageText_Implementation(float DamageAmount, bool bIsCritical, AActor* DamagedActor);


	bool bIsCastingSkill = false;
	//스킬 시전 시 방향 저장
	FVector SavedSkillLocation;
public:
	TObjectPtr<UAnimBase> GetAnimBase();
	TObjectPtr<UAbilityComponent> GetAbilityComp();
	TObjectPtr<USkillComponent> GetSkillComp();
	void SetIsCastingCheck(bool IsCasting);

	//언리얼 에디터에서 작업 MontageMapping 맴버인 MontageID 는 스킬이름이랑 동일하게 설정 ,  에디터에서 해당 스킬몽타주 등록
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Replicated, Category = "Animation")
	TArray<FMontageMapping> SkillMontages;

	UFUNCTION()
	UAnimMontage* GetSkillMontage(FName montageID);

	//server에서 호출 NetMulticast함수 호출용도 모든 클라이언트에게 동기화
	void MontageAction(UAnimMontage* montage, bool IsSkillCast);

	// 데미지 텍스트 위젯 클래스 (WBP_DamageText 할당용)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWidget_FloatingDamage> DamageTextWidgetClass;

protected:
	virtual void BeginPlay() override;



public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual float TakeDamage(float DamageAmount, AActor* DamageCauser);
			
	virtual void SetCharacterData(FName& id);
	virtual void SetSaveSkillLocation(FVector Location);
	virtual FVector GetSaveSkillLocation();
	virtual void HandleAttackAnimationEnded(bool bInterruped);
	// BeginPlay에서 호출될 캐릭터 데이터 초기화 함수

	//Anim 변수를 해당 상속받은 클래스 타입으로 캐스팅 하기 위함 애님인스턴스 호출시 틱 1번 돌아야함
	//Begin에서 타임이 되면 여기 클래스의 InitializeAnimationInstance를 바인딩 해놨지만 virtual로 자식에서 재정의하면
	//자식 함수가 호출됨



#pragma region Ability
public:
	//디폴트어빌리티 전용함수
	void SetDefaultAbil(const TArray<FAbilityData>& abils); // <- TArray 값 전체가 들어오는 함수(덮어씀)
	void ApplyDefaultAbil(const FAbilityData& abil);		// <- Default 카테고리 값 수치 조정

	//====디폴트값, Total값은 사용 불가능==============================
	void AddAbilData(eAbilCategoryType type, const TArray<FAbilityData>& abils);
	void AddAbilData(eAbilCategoryType type, const FAbilityData& abil);
	void EraseAbilData(eAbilCategoryType category, FName TID);
	//=================================================================
	//Default, Total Ability만 조회가능
	const FAbilityData* GetAbility(eAbilCategoryType category, eAbilType type);


	
	//HP, MP 증가 감소 로직(피해 ,포션회복 등) 서버에서 사용해야함
	void ApplyHP(float hp);
	void ApplyMP(float mp);
	//서버에서 호출해야함
	UFUNCTION(BlueprintCallable)
	float GetHP();
	UFUNCTION(BlueprintCallable)
	float GetMP();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	float GetHPRatio_Safe();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	float GetMPRatio_Safe();

		
#pragma endregion

#pragma region Skill
	bool SkillAction(FName& skillID);  //내가 가지고 있는 스킬 실행


	//스킬 취소 버튼 눌렀을때
	void CancelCurrentSkill();
	//스킬 사용 가능한지 체크
	FSkillData* IsUsedSkill(FName& skillTID); 
	//사용할 스킬 세팅
	void SetSkillData(FName& tid);
	//스킬을 12345 등 UI세팅
	void SetSkillSlot(int32 slot, FName TID);
	//슬롯 스킬 해제
	void EraseSkillSlot(int32 slot);
	virtual void RefreshSkillCoolDown(FName& skillID);

	//스킬데이터 SkillAction으로 넘길때 사용함 =====
	struct FSkillTableRow GetCurrentSkillData();       //임의 사용 금지 
	FName GetCurrentSkillTID();

	//==============================================
#pragma endregion

	void SetAnimationInstance(FName& animTID);

	virtual void AddReward(struct FRewardTableRow* row);

	void SetTagNames(TArray<FName>& tagIDs);

	// 자식(NPC)에서 오버라이드할 수 있도록 빈 가상 함수를 만듭니다.
	virtual void Interact(AActor* Interactor) {}

	virtual void EquipItem(FItemEquipData& data);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopMontage(float InblendOutTIme);
	void Multicast_StopMontage_Implementation(float InblendOutTIme);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* montage, bool SkillCast);
	void Multicast_PlayMontage_Implementation(UAnimMontage* montage, bool SkillCast);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
public:
		// 무기 메쉬 컴포넌트
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		UStaticMeshComponent* Weapon_R_MeshComp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		UStaticMeshComponent* Weapon_L_MeshComp;

		// 헬멧 (머리)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		UStaticMeshComponent* HelmetMeshComp;

		// 갑옷 (몸통 - 보통은 SkeletalMesh를 교체하지만, 간단히 부착물로 가정)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		UStaticMeshComponent* ArmorMeshComp;

		// 장비 외형 업데이트 함수
		void UpdateWeaponVisual(const FName& ItemTID, eItemEquipType Type);
		
		// 장비 외형 업데이트 함수(통합)
		void UpdateEquipmentVisuals();

private:
	void Weapon_R_MeshSetting(const FName& TID);
	void Weapon_L_MeshSetting(const FName& TID);
	void Helmet_MeshSetting(const FName& TID);
	void Armor_MeshSetting(const FName& TID);
};
