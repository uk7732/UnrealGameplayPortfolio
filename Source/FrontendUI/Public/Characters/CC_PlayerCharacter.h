// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CC_CharacterBase.h"
#include "CC_NPCCharacter.h"
#include "InputActionValue.h" 

#include "CC_PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

class USpringArmComponent;
class UCameraComponent;
class UCameraOcclusionComponent;
class UInputAction;
class UWidget_ActivatableBase;

UCLASS()
class FRONTENDUI_API ACC_PlayerCharacter : public ACC_CharacterBase
{
	GENERATED_BODY()

public:
	ACC_PlayerCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerTID)
	FName PlayerTID;

	// 카메라를 캐릭터에 고정하고 거리를 조절해주는 스프링 암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	// 실제 플레이어가 보게 될 시점을 담는 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	//입력액션 매핑 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	//TObjectPtr<class UDefaultInputSystem> InputSystem;
	TObjectPtr<class UDefaultInputSystem> InputSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> ReloadActionMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraOcclusionComponent> UCameraOcclusion;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;


protected:
	//마우스커서(시점)과 이동방향 사이 각도 블렌드스페이스에 적용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float DirectionAngle;

	//현재 이동속도 블렌드스페이스에 적용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float CurrentSpeed;
	//플레이어 이동방향과 커서방향 각도구함
	void CalculateDirectionAngle(float DeltaTime);
	//플레이어 속도 구함 (MoveComponent->Velocity 사용으로 자동 리플리케이트 되게 하기 위함임)
	void CalculateCurrentSpeed();
	void UpdateMovementSpeed(float DeltaTime);
	void RotReplicateToServer(float DeltaTime);

	//회전 업데이트(서버 전송) 주기 체크 함수
	float RotationUpdateTime = 0.f;
	FRotator LastSentRotation = FRotator::ZeroRotator;
	//몽타주 사용 여부
	bool bIsPlayingMontage = false;


protected:
	virtual void BeginPlay() override;
	void SetDefaultValueData();

protected:
	// ESC 입력 처리 함수
	void Input_Pause(const FInputActionValue& Value);

	// [Enhanced Input] Pause 액션 (BP에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InputAction_Pause;

	// [UI] 띄울 위젯 클래스 (WBP_PauseMenu 할당)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftClassPtr<UWidget_ActivatableBase> PauseMenuWidgetClass;

#pragma region Input
public:
	//=====Move ======
	void MoveAction(const struct FInputActionValue& Value);
	void MoveStopAction(const struct FInputActionValue& Value);
	//================
	
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& Hit) override;
	
	//=====Sprint=====
	void StartSprintingAction();
	void StopSprintingAction();
	void ConsumeSprintMana();

	// 카메라 세이크용
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	//=====Dash======
	void StartDashAction();
	UFUNCTION()
	void StopDashAction();
	float DashCostMana = 30.0f;
	void SetDashCostMana(float CostMana);

	FTimerHandle DashTimerHandle; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsDashing) // 서버에서 바뀌면 클라이언트 함수를 호출해라!
	bool bIsDashing = false;

	UFUNCTION()
	void OnRep_IsDashing();

	//구현 하지 마세요
	UFUNCTION(BlueprintImplementableEvent, Category = "Dash")
	void StartDashParticle();
	//구현 금지
	UFUNCTION(BlueprintImplementableEvent, Category = "Dash")
	void StopDashParticle();

	//=====Reload=====
	void ReloadAction(const struct FInputActionValue& Value);
	bool bIsReloadAction = false;
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmoCount();
	void SetCurrentAmmoCount(int32 ammo);
	void UsedCurrentAmmo();
	int32 GetMaxAmmoCount();

	// 상호작용
	void InteractAction(const FInputActionValue& Value);

	// 포션 사용
	void Input_UsePotion(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void RefreshCameraAngle(FRotator Rotator);
private:
	//쉬프트 누르고 있는동안 1초마다 마나소모 
	FTimerHandle TimerHandle_SprintManaCost;
	//================



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int32 CurrentAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int32 MaxAmmoCount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float SprintStartRotationRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float SprintStopRotationRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float SprintSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float WalkSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float SpeedInterpRate = 0.f;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input", ReplicatedUsing = OnRep_SprintUpdate)
	bool bIsSprinting = false;

#pragma endregion 

public:
	TObjectPtr<USpringArmComponent> GetSpringArm();

	virtual void SetPlayerData(FName& id); //플레이어 설정
	virtual void SetSavePlayerData(FName& id); //플레이어 저징용 

	virtual void AddReward(struct FRewardTableRow* row); //보상 추가(아이템 등)
	void SetCurrentSpeed(float speed) { CurrentSpeed = speed; }
	void ChangeAnimation(eAnimType anim);

	// 인벤토리 테스트
	void Debug_FillInventory(const struct FInputActionValue& Value);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChanged OnAmmoChanged;

	UFUNCTION(Server, Reliable)
	void Server_Debug_FillInventory();
	void Server_Debug_FillInventory_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_Interact(ACC_NPCCharacter* TargetNPC);
	void Server_Interact_Implementation(ACC_NPCCharacter* TargetNPC);
#pragma region Skill
public:
	void NormalAction(const struct FInputActionValue& Value);
	void SubNormalAction(const struct FInputActionValue& Value);

	void InputSkill_Number01(const struct FInputActionValue& Value);
	void InputSkill_Number02(const struct FInputActionValue& Value);

	void Input_Chat(const struct FInputActionValue& Value);

	//스킬슬롯 지정, 해제
	UFUNCTION(BlueprintCallable)
	void SetSkillSlot(int32 SlotNum, FName SkillTID);

	UFUNCTION(BlueprintCallable)
	void EraseSkillSlot(int32 SlotNum);

#pragma endregion

	// 인벤토리 실행
	void Input_Inventory(const struct FInputActionValue& Value);

	//캡슐컴포넌트에 충돌이 났을때 호출되는 함수
	UFUNCTION()
	void OnPlayerBodyCollisionCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//입력을 바인딩하기위해 호출함
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PlayerTID();

	UFUNCTION()
	void OnRep_SprintUpdate();

	UFUNCTION()
	void OnRep_CurrentAmmoCount();
};