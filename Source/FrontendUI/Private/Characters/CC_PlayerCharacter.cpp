// Vince Petrelli All Rights Reserved

#include "Characters/CC_PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "EnhancedInputSubsystems.h"
#include "Input/DefaultInputSystem.h"
#include "Blueprint/UserWidget.h"
#include "Instance/TableSubsystem.h"
#include "Controllers/CC_PlayerController.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Table/TableDatas.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Common/PacketDatas.h"
#include "Component/AbilityComponent.h"
#include "Component/SkillComponent.h"
#include "Component/InventoryComponent.h"
#include "GameMode/CC_PlayerState.h"
#include "Characters/CC_NPCCharacter.h"
#include "FrontendDebugHelper.h"
#include "DrawDebugHelpers.h"
#include "Component/CameraOcclusionComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Subsytems/FrontendUISubsystem.h" 
#include "FrontendGameplayTags.h"          
#include "Widgets/Widget_ActivatableBase.h"
#include "Camera/CameraShakeBase.h"          

ACC_PlayerCharacter::ACC_PlayerCharacter()
{
    // Tick은 필요없음 (PlayerController에서 회전 처리)
    PrimaryActorTick.bCanEverTick = true;

    UCapsuleComponent* capsuleComp = GetCapsuleComponent();

    // 1. Custom Depth 활성화 (실루엣 렌더링용)
    GetMesh()->SetRenderCustomDepth(true);

    // 1-2. Stencil Value 설정 (예: 1 = 플레이어/아군)
    GetMesh()->SetCustomDepthStencilValue(1);

    // 2. 캡슐 사이즈는 유지 (내 코드와 대상 코드 모두 없으므로 추가)
    GetCapsuleComponent()->InitCapsuleSize(42.f, 72.f);

    // 3. 회전 설정 (내 코드의 탑다운 설정 적용)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    Debug::Print(TEXT(""), -1, FColor::Red);
    // 4. 이동 설정 (내 코드의 설정 적용)
    GetCharacterMovement()->bOrientRotationToMovement = false;
    //  수동 회전을 위해 Controller Desired Rotation 비활성화
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 99999.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.25f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

    // 5. 컴포넌트 생성 및 부착 (대상 코드의 변수 이름 사용)
    // **[주의]** 대상 코드 헤더의 변수 이름이 SpringArm, Camera로 가정합니다.
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetupAttachment(RootComponent); // GetRootComponent() 사용
    

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); // FollowCamera로 생성
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    
    // **[이름 충돌]** 대상 코드의 Camera 변수에 FollowCamera를 저장합니다.
    // 만약 헤더에 FollowCamera라는 변수가 따로 있다면, 이름을 맞춰주세요.
    // 현재는 대상 코드의 Camera 변수를 사용합니다.
    // TObjectPtr<UCameraComponent> Camera; // 대상 코드 헤더 변수

    // 6. 스프링 암 설정 (내 코드의 탑다운 설정 적용)
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    // 7. 카메라 설정
    Camera->bUsePawnControlRotation = false;

    UCameraOcclusion = CreateDefaultSubobject<UCameraOcclusionComponent>(TEXT("cameraOcclusion"));

    // 8. 입력 시스템 객체 생성 (대상 코드 유지)
//    InputSystem = CreateDefaultSubobject<UDefaultInputSystem>(TEXT("InputSystem"));

}

void ACC_PlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (PlayerTID.IsNone())
        {
            UTableSubsystem* tablesubsystem = UTableSubsystem::Get(this);
            if (nullptr == tablesubsystem)
                return;

            FDefineTableRow* definerow = tablesubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::Player_DefaultTID);
            if (nullptr == definerow)
                return;

            FName defaultID = FName(definerow->StrVal);
            SetPlayerData(defaultID);
        }

        SetDefaultValueData();
    }

    UCapsuleComponent* capsule = GetCapsuleComponent();
    if (capsule)
        capsule->OnComponentBeginOverlap.AddDynamic(this, &ACC_PlayerCharacter::OnPlayerBodyCollisionCallback);

    const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

    // 로컬 플레이어일 때만 타이머 가동 내 카메라 투명도 체크주기 로직임
    CurrentAmmoCount = MaxAmmoCount;
}

void ACC_PlayerCharacter::SetDefaultValueData()
{
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    FDefineTableRow* SprintStart_rotationrate = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::SprintStart_RotationRate);
    if (nullptr == SprintStart_rotationrate)
        return;

    FDefineTableRow* SprintStop_rotationrate = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::SprintStop_RotationRate);
    if (nullptr == SprintStop_rotationrate)
        return;

    SprintStartRotationRate = SprintStart_rotationrate->FloatVal;
    SprintStopRotationRate = SprintStop_rotationrate->FloatVal;

    FDefineTableRow* sprintspeed = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::SprintSpeed);
    if (nullptr == sprintspeed)
        return;

    SprintSpeed = sprintspeed->FloatVal;

    FDefineTableRow* walkspeed = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::WalkSpeed);
    if (nullptr == walkspeed)
        return;
    WalkSpeed = walkspeed->FloatVal;

    FDefineTableRow* speedinterprate = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::SpeedInterpRate);
    if (nullptr == speedinterprate)
        return;

    SpeedInterpRate = speedinterprate->FloatVal;

    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        MovementComp->MaxWalkSpeed = WalkSpeed;
        // 디버깅용 로그
        UE_LOG(LogTemp, Warning, TEXT("Server: Initial MaxWalkSpeed set to: %f"), MovementComp->MaxWalkSpeed);
    }
}

void ACC_PlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RotationUpdateTime += DeltaTime;


    if (IsLocallyControlled())
    {
        RotReplicateToServer(DeltaTime);

        //이동방향과 커서방향 각도차이 구한 뒤 맴버변수에 저장
        CalculateDirectionAngle(DeltaTime);

        //현재 이동속도 변수에 저장
        CalculateCurrentSpeed();
    }
}

void ACC_PlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

}

TObjectPtr<USpringArmComponent> ACC_PlayerCharacter::GetSpringArm()
{
    return SpringArm;
}

void ACC_PlayerCharacter::SetPlayerData(FName& id)
{
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    PlayerTID = id;

    if (PlayerTID.IsNone())
    {
        FDefineTableRow* defineRow = tableSubsystem->FindTableRow<FDefineTableRow>(TABLE_NAME::DEFINE, DEFINE_NAME::Player_DefaultTID);
        if (nullptr == defineRow)
            return;

        PlayerTID = FName(defineRow->StrVal);

    }

    FPlayerTableRow* playerRow = tableSubsystem->FindTableRow<FPlayerTableRow>(TABLE_NAME::PLAYER, PlayerTID);
    if (nullptr == playerRow)
        return;

    USkeletalMeshComponent* meshComp = GetMesh();

    FAnimationTableRow* AnimTable = tableSubsystem->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, FName(playerRow->AnimationTID));
    if (nullptr == AnimTable)
        return;


    meshComp->SetAnimInstanceClass(AnimTable->Animation);

    auto animInst = meshComp->GetAnimInstance();

    Anim = Cast<UAnimBase>(meshComp->GetAnimInstance());

    Anim->SetAnimType(eAnimType::ANIM_TYPE_IDLE);


    FAbilGroupTableRow* abilGroupRow = tableSubsystem->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, playerRow->AbilGroupTID);
    if (nullptr == abilGroupRow)
        return;

    SetDefaultAbil(abilGroupRow->Abils);

    for (auto& it : playerRow->SkillIDs)
        SetSkillData(it);

    FTagTableRow* tagRow = tableSubsystem->FindTableRow<FTagTableRow>(TABLE_NAME::TAG, playerRow->TagTID);
    if (nullptr == tagRow)
        return;

    SetTagNames(tagRow->Tags);
}

void ACC_PlayerCharacter::SetSavePlayerData(FName& id)
{
    //todo 저장시 로직 짜기
}

void ACC_PlayerCharacter::AddReward(struct FRewardTableRow* row)
{

}

void ACC_PlayerCharacter::ChangeAnimation(eAnimType anim)
{
    if (false == IsValid(Anim))
        return;

    Anim->SetAnimType(anim);
}

void ACC_PlayerCharacter::InputSkill_Number01(const FInputActionValue& Value)
{
    // 1. 컨트롤러 확인
    ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController());
    if (nullptr == PC) return;

    // 2. 스킬 컴포넌트 유효성 및 1번 슬롯 스킬 확인
    if (Skill)
    {
        // 아까 만든 Getter 함수 사용 (1번 슬롯)
        FName SkillTID = Skill->GetSkillTIDInSlot(1);

        // 3. 등록된 스킬이 있다면 서버로 시전 요청
        if (!SkillTID.IsNone())
        {
            SkillAction(SkillTID);
        }
    }
    Debug::Print(TEXT("스킬 사용"), -1, FColor::Cyan);
}

void ACC_PlayerCharacter::InputSkill_Number02(const FInputActionValue& Value)
{
    // 1. 컨트롤러 확인
    ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController());
    if (nullptr == PC) return;

    // 2. 스킬 컴포넌트 유효성 및 2번 슬롯 스킬 확인
    if (Skill)
    {
        // 아까 만든 Getter 함수 사용 (2번 슬롯)
        FName SkillTID = Skill->GetSkillTIDInSlot(2);

        // 3. 등록된 스킬이 있다면 서버로 시전 요청
        if (!SkillTID.IsNone())
        {
            SkillAction(SkillTID);
        }
    }
}

void ACC_PlayerCharacter::Input_Chat(const FInputActionValue& Value)
{
    ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController());
    if (PC)
    {
        // 컨트롤러에 구현할 함수 호출
        PC->FocusChat();
    }
}

void ACC_PlayerCharacter::OnPlayerBodyCollisionCallback(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    //OverlappedComponent - 충돌이 발생한 주체 컴포넌트(나 자신) 이 경우엔 ACC_PlaerCharacter의 UCapsulComponent가 됨
    //OtherActor - 오버랩된 상대 액터 
    //OtherComp - 오버랩된 상대 컴포넌트
    //OtherBodyIndex - 오버랩된 상대 컴포넌트 내의 바디인덱스 (여러개의 물리바디를 가질때 사용)
    //bFromSweep - 이 충돌이 Sweep 테스트의 결과인지 true 이면 이동함수 
    //SweepResult - Sweep테스트를 통해 얻은 충돌 정보 구조체
    //sweep테스트란 ? 움직임이 벽이나 다른 물체에 의 해막히는지 예측하고 어느지점에 어떤각도로 충돌했는지에 대한 정보를 얻음

    if (OtherActor->Tags.Contains("Monster_Skill"))
    {
        //todo : monster collision 구현하기
    }

    if (OtherActor->Tags.Contains("Item"))
    {
        // item 충돌 시 반응 구현하기
    }


}

void ACC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // EnhancedInputComponent로 형변환합니다.
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // PlayerController를 가져옵니다.
        if (ACC_PlayerController* PlayerController = Cast<ACC_PlayerController>(GetController()))
        {
            // 로컬 플레이어의 EnhancedInput 서브시스템을 가져옵니다.
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
            {
                InputSystem = NewObject<UDefaultInputSystem>(this);

                // 서브시스템에 우리의 매핑 컨텍스트(IMC_Default)를 추가합니다.
                Subsystem->AddMappingContext(InputSystem->Context, 0);
            }

            // 입력 액션과 처리 함수를 '바인딩'합니다.
            // Move 액션이 'Triggered'될 때마다 Move 함수를 호출
            //EnhancedInputComponent->BindAction(InputSystem->Move, ETriggerEvent::Triggered, PlayerController, &ACC_PlayerController::server_StartMoveAction);
            //EnhancedInputComponent->BindAction(InputSystem->Move, ETriggerEvent::Completed, PlayerController, &ACC_PlayerController::server_StopMoveAction);

            EnhancedInputComponent->BindAction(InputSystem->Move, ETriggerEvent::Triggered, this, &ACC_PlayerCharacter::MoveAction);
            EnhancedInputComponent->BindAction(InputSystem->Move, ETriggerEvent::Completed, this, &ACC_PlayerCharacter::MoveStopAction);

            // Fire 액션이 'Triggered'될 때마다 Fire 함수를 호출
            //EnhancedInputComponent->BindAction(InputSystem->Fire, ETriggerEvent::Triggered, this, &ACC_BaseCharacter::Fire);
            // Jump 액션이 'Triggered'될 때마다 Character 클래스에 내장된 Jump 함수를 호출
            //EnhancedInputComponent->BindAction(InputSystem->Jump, ETriggerEvent::Triggered, PlayerController, &ACC_PlayerController::server_Jump);
            EnhancedInputComponent->BindAction(InputSystem->Jump, ETriggerEvent::Triggered, this, &ACC_PlayerCharacter::Jump);
            
            // Jump 액션이 'Completed'될 때마다 Character 클래스에 내장된 StopJumping 함수를 호출
            //EnhancedInputComponent->BindAction(InputSystem->Jump, ETriggerEvent::Completed, PlayerController, &ACC_PlayerController::server_StopJumping);
            EnhancedInputComponent->BindAction(InputSystem->Jump, ETriggerEvent::Completed, this, &ACC_PlayerCharacter::StopJumping);

            EnhancedInputComponent->BindAction(InputSystem->NormalSkillAction, ETriggerEvent::Started, PlayerController, &ACC_PlayerController::server_NormalSkillAction);

            EnhancedInputComponent->BindAction(InputSystem->SubNormalSkillAction, ETriggerEvent::Started, PlayerController, &ACC_PlayerController::server_SubNormalSkillAction);
         
            EnhancedInputComponent->BindAction(InputSystem->Reload, ETriggerEvent::Triggered, PlayerController, &ACC_PlayerController::server_ReloadAction);

            EnhancedInputComponent->BindAction(InputSystem->Sprint, ETriggerEvent::Started, PlayerController, &ACC_PlayerController::server_StartSprintAction);

            EnhancedInputComponent->BindAction(InputSystem->Sprint, ETriggerEvent::Completed, PlayerController, &ACC_PlayerController::server_StopSprintAction);

            EnhancedInputComponent->BindAction(InputSystem->Dash, ETriggerEvent::Started, PlayerController, &ACC_PlayerController::server_StartDashAction);

            EnhancedInputComponent->BindAction(InputSystem->Skill1, ETriggerEvent::Started, this, &ACC_PlayerCharacter::InputSkill_Number01);
            //EnhancedInputComponent->BindAction(InputSystem->Skill2, ETriggerEvent::Started, this, &ACC_PlayerCharacter::InputSkill_Number02);

            EnhancedInputComponent->BindAction(InputSystem->Skill2, ETriggerEvent::Started, this, &ACC_PlayerCharacter::Debug_FillInventory);

            EnhancedInputComponent->BindAction(InputSystem->Inventory, ETriggerEvent::Started, this, &ACC_PlayerCharacter::Input_Inventory);

            EnhancedInputComponent->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ACC_PlayerCharacter::InteractAction);

            EnhancedInputComponent->BindAction(InputSystem->Potion_HP, ETriggerEvent::Started, this, &ACC_PlayerCharacter::Input_UsePotion);

            EnhancedInputComponent->BindAction(InputSystem->Chat, ETriggerEvent::Started, this, &ACC_PlayerCharacter::Input_Chat);

            EnhancedInputComponent->BindAction(InputSystem->Pause, ETriggerEvent::Started, this, &ACC_PlayerCharacter::Input_Pause);
        }
    }
}

void ACC_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACC_PlayerCharacter, PlayerTID);
    DOREPLIFETIME(ACC_PlayerCharacter, SprintStartRotationRate);
    DOREPLIFETIME(ACC_PlayerCharacter, SprintStopRotationRate);
    DOREPLIFETIME(ACC_PlayerCharacter, SprintSpeed);
    DOREPLIFETIME(ACC_PlayerCharacter, WalkSpeed);
    DOREPLIFETIME(ACC_PlayerCharacter, SpeedInterpRate);
    DOREPLIFETIME(ACC_PlayerCharacter, bIsSprinting);
    DOREPLIFETIME(ACC_PlayerCharacter, CurrentAmmoCount);
    DOREPLIFETIME(ACC_PlayerCharacter, MaxAmmoCount);
    DOREPLIFETIME(ACC_PlayerCharacter, bIsDashing);
}

void ACC_PlayerCharacter::MoveAction(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator Rotation = SpringArm->GetRelativeRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);

    ChangeAnimation(eAnimType::ANIM_TYPE_WALK);
}

void ACC_PlayerCharacter::MoveStopAction(const FInputActionValue& Value)
{
    ChangeAnimation(eAnimType::ANIM_TYPE_IDLE);
}

void ACC_PlayerCharacter::Jump()
{
    Super::Jump();

    if (false == IsValid(Anim)) return;

    FName jump = "Jump";
    UAnimMontage* playmontage = GetSkillMontage(jump);
    if (nullptr == playmontage) return;

    MontageAction(playmontage, false);
}

void ACC_PlayerCharacter::StopJumping()
{
    Super::StopJumping();
}

void ACC_PlayerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (false == IsValid(Anim)) return;
    
    FName land = "Land";
    UAnimMontage* PlayMontage = GetSkillMontage(land);

    if (PlayMontage)
    {
        MontageAction(PlayMontage, false);
    }
}

void ACC_PlayerCharacter::Server_Interact_Implementation(ACC_NPCCharacter* TargetNPC)
{
    if (TargetNPC)
    {
        // 거리 체크 (보안상 한번 더 확인)
        float Distance = FVector::Dist(GetActorLocation(), TargetNPC->GetActorLocation());
        if (Distance > 300.0f) return; // 너무 멀면 무시

        // 서버에서 NPC의 Interact 함수를 호출합니다.
        // 이제 NPC 내부의 GameInstance->HostMultiplayer가 정상 작동합니다.
        TargetNPC->Interact(this);
    }
}

void ACC_PlayerCharacter::NormalAction(const FInputActionValue& Value)
{
    if (bIsSprinting)
        return;

    FName NormalSkillTID = Skill->GetNormalSkillTID();

    SkillAction(NormalSkillTID);
}

void ACC_PlayerCharacter::SubNormalAction(const FInputActionValue& Value)
{
    FName SubNormalSkillTID = Skill->GetSubNormalSkillTID();
    
    SkillAction(SubNormalSkillTID);
}


void ACC_PlayerCharacter::StartSprintingAction()
{
    if (!HasAuthority())
        return;

    float CurrentMana = GetMP();
    if (CurrentMana < 5.0f)
        return;


    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        bIsSprinting = true;

        bUseControllerRotationYaw = false;

        MovementComp->MaxWalkSpeed = SprintSpeed;

        // 컨트롤러(마우스 시선) 회전 무시
        MovementComp->bUseControllerDesiredRotation = false;
        // 이동방향을 따라 캐릭터가 회전하도록 설정
        MovementComp->bOrientRotationToMovement = true;
        // 회전속도 설정
        MovementComp->RotationRate = FRotator(0.0f, SprintStartRotationRate, 0.0f);
    }

    GetWorldTimerManager().SetTimer(
        TimerHandle_SprintManaCost,
        this,
        &ACC_PlayerCharacter::ConsumeSprintMana,
        1.0f,
        true
    );
}


void ACC_PlayerCharacter::StopSprintingAction()
{
    if (!HasAuthority())
        return;

    // 타이머가 활성화되어 있다면 중지
    GetWorldTimerManager().ClearTimer(TimerHandle_SprintManaCost);


    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        bIsSprinting = false;
        bUseControllerRotationYaw = false;

        MovementComp->MaxWalkSpeed = WalkSpeed;
        MovementComp->bUseControllerDesiredRotation = true;
        MovementComp->bOrientRotationToMovement = false;
        MovementComp->RotationRate = FRotator(0.0f, SprintStopRotationRate, 0.0f);
    }
}

void ACC_PlayerCharacter::ConsumeSprintMana()
{
    if (!HasAuthority())
    {
        GetWorldTimerManager().ClearTimer(TimerHandle_SprintManaCost);
        return;
    }

    float CurrentMana = GetMP();
    float SprintManaCost = 5.0f;
    if (CurrentMana >= SprintManaCost)
    {
        //마나소모
        ApplyMP(-SprintManaCost);
        UE_LOG(LogTemp, Warning, TEXT("마나 소모 : %.1f 남음"), CurrentMana = SprintManaCost);
    }
    else
    {
        ACC_PlayerController* controller = Cast<ACC_PlayerController>(GetController());
        if (controller)
        {
            controller->server_ForceStopSprint();
        }
    }
}


void ACC_PlayerCharacter::StartDashAction()
{
    if (!HasAuthority())
        return;

    if (bIsDashing)
        return;
    
    float MP = GetMP();

    if (MP <= DashCostMana)
        return;

    ApplyMP(-DashCostMana);

    bIsDashing = true;

    OnRep_IsDashing();

    // 이동 컴포넌트가 유효한지 확인
    if (GetCharacterMovement())
    {
        FVector DashDirection = GetActorForwardVector();
        float DashStrength = 20000.f; 

        // XYOverride를 true로 주어 순간적인 대시 구현
        LaunchCharacter(DashDirection * DashStrength, true, false);
    }

    ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController());
    
    if(PC)
        GetWorldTimerManager().SetTimer(DashTimerHandle, PC, &ACC_PlayerController::server_StopDashAction, 0.2f, false);
}

void ACC_PlayerCharacter::StopDashAction()
{
    bIsDashing = false;

    GetWorldTimerManager().ClearTimer(DashTimerHandle);
}

void ACC_PlayerCharacter::SetDashCostMana(float CostMana)
{
    DashCostMana = CostMana;
}

void ACC_PlayerCharacter::OnRep_IsDashing()
{
    if (bIsDashing)
    {
        StartDashParticle();
    }
    else
    {
        StopDashParticle();
    }
}


void ACC_PlayerCharacter::ReloadAction(const FInputActionValue& Value)
{

    // 1. 이미 재장전 중이거나 다른 스킬 사용 중이면 리턴
    if (!Skill->GetCurrentSkillTID().IsNone())
        return;

    // 2. 탄창이 이미 꽉 찼으면 재장전 안 함
    if (CurrentAmmoCount >= MaxAmmoCount) return;

    if (Skill->GetCurrentSkillTID() == FName("ReloadAction"))
    {
        Skill->CancelCurrentSkill();
        return;
    }

    // 3. 인벤토리에 탄약이 없으면 재장전 안 함
    ACC_PlayerState* PS = GetPlayerState<ACC_PlayerState>();
    if (PS)
    {
        FItemData AmmoItem = PS->GetInventory()->GetAmmoData();
        if (AmmoItem.Count <= 0) return; // 탄약 없음
    }

    ACC_PlayerState* Playerstate = Cast<ACC_PlayerState>(GetPlayerState());
    if (nullptr == Playerstate)
        return;

    FItemData AmmoData = Playerstate->GetInventory()->GetAmmoData();

    if (CurrentAmmoCount == MaxAmmoCount || AmmoData.Count == 0)
        return;

    if (nullptr == ReloadActionMontage)
        return;

    // 4. 재장전 애니메이션(몽타주) 재생
    Skill->SetCurrentSkillTID(FName("ReloadAction"));

    MontageAction(ReloadActionMontage, true);
    
    //[핵심] 서버에 "인벤토리 탄약 -> 총으로 이동" 요청
    if (Skill)
    {
        Skill->Server_RequestReload();
    }
}

int32 ACC_PlayerCharacter::GetCurrentAmmoCount()
{
    return CurrentAmmoCount;
}

void ACC_PlayerCharacter::SetCurrentAmmoCount(int32 ammo)
{
    CurrentAmmoCount += ammo;

    CurrentAmmoCount = FMath::Min(MaxAmmoCount, CurrentAmmoCount);

    // UI에 알림
    if (OnAmmoChanged.IsBound())
        OnAmmoChanged.Broadcast(CurrentAmmoCount, MaxAmmoCount);

}

void ACC_PlayerCharacter::UsedCurrentAmmo()
{
    // 탄약이 0이면 줄이지 않음
    if (CurrentAmmoCount <= 0)
    {
        // 디버깅용: 0발이라 못 줄였다는 로그
        UE_LOG(LogTemp, Error, TEXT("[UsedCurrentAmmo] 탄약이 0이라 감소 실패!"));
        return;
    }

    CurrentAmmoCount--;

    // 서버(Host) 플레이어의 경우 OnRep가 호출되지 않으므로 여기서 직접 호출해야 함
    if (OnAmmoChanged.IsBound())
    {
        OnAmmoChanged.Broadcast(CurrentAmmoCount, MaxAmmoCount);
    }
}

int32 ACC_PlayerCharacter::GetMaxAmmoCount()
{
    return MaxAmmoCount;
}

void ACC_PlayerCharacter::InteractAction(const FInputActionValue& Value)
{
    {
        // 1. 입력 확인 로그
        UE_LOG(LogTemp, Warning, TEXT("[Input] G Key Pressed"));

        FVector Start = GetActorLocation();
        FVector End = Start + (GetActorForwardVector() * 300.0f); // 거리를 150 -> 300으로 늘려보세요 (너무 짧을 수 있음)

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        // 2. 눈에 보이는 레이저 발사 (디버그 라인)
        // 빨간색 선이 나갑니다. NPC에 닿으면 초록색 점이 생겨야 합니다.
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult, Start, End, ECC_Visibility, Params
        );

        if (bHit)
        {
            AActor* HitActor = HitResult.GetActor();
            if (HitActor)
            {
                // 무엇에 맞았는지 이름 출력
                UE_LOG(LogTemp, Warning, TEXT("[Trace] Hit Actor: %s"), *HitActor->GetName());

                ACC_NPCCharacter* NPC = Cast<ACC_NPCCharacter>(HitActor);
                if (NPC)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Trace] Actor is NPC! Requesting Server Interaction..."));
                    Server_Interact(NPC);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("[Trace] Hit Actor is NOT an NPC class."));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Trace] No Hit! (허공을 가름)"));
        }
    }
}

float ACC_PlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 1. 기존 기능(부모) 실행 (절대 빼먹으면 안 됨!)
    // 이 한 줄이 질문자님이 올리신 그 긴 코드를 대신 실행해줍니다.
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 2. 새로운 기능(카메라 흔들기) 추가
    if (ActualDamage > 0.0f && IsLocallyControlled())
    {
        if (HitCameraShakeClass)
        {
            if (ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController()))
            {
                PC->ClientStartCameraShake(HitCameraShakeClass, 1.0f);
            }
        }
    }

    return ActualDamage;
}

void ACC_PlayerCharacter::Input_UsePotion(const FInputActionValue& Value)
{

    float CurrentHP = 0.0f;
    float MaxHP = 0.0f;

    // Ability 컴포넌트가 유효하다면 값 가져오기
    if (Ability)
    {
        CurrentHP = Ability->GetHP();
        MaxHP = Ability->GetMaxHP();
    }

    // [중요] 문자열 포맷팅 (예: "포션사용 시도 (HP: 90.0 / 100.0)")
    // %.1f는 소수점 첫째 자리까지만 표시하라는 뜻입니다.
    FString Msg = FString::Printf(TEXT("포션사용 시도 (HP: %.1f / %.1f)"), CurrentHP, MaxHP);

    // 화면에 출력
    Debug::Print(Msg, -1, FColor::Green);

    // 출력 로그(Output Log)에도 남기기
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

    // 서버 RPC가 아니어도 InventoryComponent의 UseItem은 내부적으로 서버 RPC를 호출합니다.
    ACC_PlayerState* PS = GetPlayerState<ACC_PlayerState>();
    if (!PS) return;

    UInventoryComponent* Inv = PS->GetInventory();
    if (!Inv) return;

    // 인벤토리를 순회하며 'Potion_HP_01'을 찾습니다.
    const TArray<FItemData>& Items = Inv->GetInventoryItems();
    for (const FItemData& Item : Items)
    {
        // TID는 데이터테이블에 등록한 포션 ID와 같아야 합니다.
        if (Item.TID == FName("Potion_HP_01"))
        {
            // 찾았으면 사용 요청 (InventoryComponent의 함수 재사용)
            Inv->Client_RequestUseItem(Item.UID);

            // 하나만 쓰고 종료
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("사용할 포션이 없습니다."));
}

void ACC_PlayerCharacter::RefreshCameraAngle(FRotator Rotator)
{

}

void ACC_PlayerCharacter::Input_Inventory(const FInputActionValue& Value)
{
    
    // UI는 PlayerController에서 관리하는 게 일반적입니다.
    ACC_PlayerController* PC = Cast<ACC_PlayerController>(GetController());
    if (PC)
    {
        // 디버그용 로그
        UE_LOG(LogTemp, Warning, TEXT("인벤토리 키 눌림!"));
        Debug::Print(TEXT("인벤토리 창 열기"), -1, FColor::Green);

        // 컨트롤러의 토글 함수 호출
        PC->ToggleInventory();  
    }    
}

void ACC_PlayerCharacter::CalculateDirectionAngle(float DeltaTime)
{
    //1. CurrentSpeed 계산 : 단순 이동 속도(Z축제외)
    FVector CurrentVelocity = GetVelocity();
    CurrentSpeed = CurrentVelocity.Size2D();

    //2. DirectionAngle 계산 : 상대 이동 방향
    if (CurrentSpeed > KINDA_SMALL_NUMBER)
    {
        // a. 이동 방향의 회전값 (Yaw)
        FRotator MovementRotation = CurrentVelocity.ToOrientationRotator();

        // b. 캐릭터의 현재 회전값 (Yaw)
        FRotator ActorRotation = GetActorRotation();

        // c. 두 Yaw 사이의 각도 차이 (상대 각도)
        // FMath::FindDeltaAngleDegrees는 결과값을 [-180, 180] 범위로 보정해 줍니다.
        DirectionAngle = FMath::FindDeltaAngleDegrees(ActorRotation.Yaw, MovementRotation.Yaw);
    }
    else
    {
        //정지했을떄는 0으로 설정
        DirectionAngle = 0.0f;
    }
}

void ACC_PlayerCharacter::CalculateCurrentSpeed()
{
    const UCharacterMovementComponent* MovementComp = GetCharacterMovement();

    if (MovementComp)
    {
        FVector CurrentVelocity = MovementComp->Velocity;
        CurrentSpeed = CurrentVelocity.Size();
    }
}

void ACC_PlayerCharacter::SetSkillSlot(int32 SlotNum, FName SkillTID)
{
    Skill->SetSkillSlot(SlotNum, SkillTID);
}

void ACC_PlayerCharacter::EraseSkillSlot(int32 SlotNum)
{
    Skill->EraseSkillSlot(SlotNum);
}
void ACC_PlayerCharacter::Debug_FillInventory(const FInputActionValue& Value)
{  
    UE_LOG(LogTemp, Warning, TEXT("1. [Client] 키 입력 감지됨 -> 서버로 요청 보냄"));

    // 서버 함수 호출 (이러면 서버에서 _Implementation 함수가 실행됨)
    Server_Debug_FillInventory();
}

void ACC_PlayerCharacter::Server_Debug_FillInventory_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("2. [Server] 요청 받음. 인벤토리 채우기 시작"));

    // [수정] 내 캐릭터(this)가 아니라 PlayerState에서 인벤토리를 찾아야 함
    APlayerState* PS = GetPlayerState();
    ACC_PlayerState* FrontendPS = Cast<ACC_PlayerState>(PS);

    if (FrontendPS)
    {
        // PlayerState에 만들어둔 Getter 함수 사용 (혹은 FindComponentByClass)
        UInventoryComponent* Inv = FrontendPS->GetInventory();
        TArray<FItemData> ItemsToAdd;
        ItemsToAdd.Add({ 0, FName("Potion_HP_01"), 5, 0 }); 
        ItemsToAdd.Add({ 0, FName("Buster"), 1, 0 });  
        ItemsToAdd.Add({ 0, FName("Mace"), 1, 0 });  
        ItemsToAdd.Add({ 0, FName("Test_Shield"), 1, 0 });  
        ItemsToAdd.Add({ 0, FName("Test_Armor"), 1, 0 });   
        ItemsToAdd.Add({ 0, FName("Test_Helmet"), 1, 0 });  
        ItemsToAdd.Add({ 0, FName("Ammo"), 30, 0 });

        // 배열 순회, 각 아이템 인벤토리 추가
        for (const FItemData& Item : ItemsToAdd)
        {
            // 서버의 TryAddItem 함수를 호출합니다. UID는 이 함수 내부에서 할당됩니다.
            Inv->TryAddItem(Item, nullptr);
        }

        UE_LOG(LogTemp, Warning, TEXT("3. [Server] 모든 종류의 테스트 아이템 추가 완료"));

        //if (Inv)
        //{
        //    // 더미 아이템 데이터 생성
        //    FItemData TestItem;
        //    TestItem.TID = FName("Potion_HP_01"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.TID = FName("Buster"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.TID = FName("Mace"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.TID = FName("Test_Shield"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.TID = FName("Test_Armor"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.TID = FName("Potion_HP_01"); // DT_Item 테이블에 이 이름이 꼭 있어야 함
        //    TestItem.Count = 1;

        //    // 24번 반복해서 넣기
        //    for (int i = 0; i < 1  ; ++i)
        //    {
        //        // 랜덤 UID 생성
        //        TestItem.UID = FMath::RandRange(100000, 999999) + i;

        //        // 아이템 추가 시도
        //        Inv->TryAddItem(TestItem, nullptr);
        //    }

        //    UE_LOG(LogTemp, Warning, TEXT("3. [Server] 인벤토리 채우기 완료 (PlayerState에서 찾음)"));
        //}
        //else
        //{
        //    UE_LOG(LogTemp, Error, TEXT("[Server] PlayerState는 찾았으나 인벤토리 컴포넌트가 없습니다!"));
        //}
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Server] PlayerState_Frontend 캐스팅 실패!"));
    }
}

void ACC_PlayerCharacter::UpdateMovementSpeed(float DeltaTime)
{
    //SkillGauge 넣기 (bIsSprinting && SkillGauge >= 1)  이부분

    float TargetSpeed = (bIsSprinting) ? SprintSpeed : WalkSpeed;

    float CurSpeed = GetCharacterMovement()->MaxWalkSpeed;

    float NewSpeed = FMath::FInterpTo(CurSpeed, TargetSpeed, DeltaTime, SpeedInterpRate);

    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void ACC_PlayerCharacter::RotReplicateToServer(float DeltaTime)
{

    ACC_PlayerController* controller = Cast<ACC_PlayerController>(GetController());
    if (!controller) return;

    FRotator NewRot = GetActorRotation();
    bool bHitSuccessful = false;

    if (!bIsSprinting)
    {
        // 1. 마우스 위치를 월드 좌표로 변환
        FVector MouseWorldLocation, MouseWorldDirection;
        if (controller->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
        {
            // 2. 라인 트레이스 설정
            FHitResult HitResult;
            FVector Start = MouseWorldLocation;
            FVector End = Start + (MouseWorldDirection * 10000.0f); // 충분히 먼 거리

            FCollisionQueryParams Params;
            // 투명화된 벽 리스트를 무시 목록에 추가
            Params.AddIgnoredActors(controller->MouseOcclusionIgnoreActors);
            Params.AddIgnoredActor(this); // 자기 자신도 무시

            // 3. 레이캐스트 실행 (Visibility 채널 사용)
            if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params))
            {
                FVector TargetLocation = HitResult.Location;
                FVector PawnLocation = GetActorLocation();

                // 평면 회전을 위해 높이 맞춤
                TargetLocation.Z = PawnLocation.Z;

                NewRot = UKismetMathLibrary::FindLookAtRotation(PawnLocation, TargetLocation);
                NewRot.Normalize();

                controller->SetControlRotation(NewRot);
                bHitSuccessful = true;
            }
        }
    }
    RotationUpdateTime += DeltaTime;
    if (RotationUpdateTime >= 0.05f)
    {
        RotationUpdateTime = 0.f;
        if (bHitSuccessful && !bIsSprinting && FMath::Abs(NewRot.Yaw - LastSentRotation.Yaw) > 0.5f)
        {
            FC_RepRotUpdate rotUpdate;
            rotUpdate.Rot = NewRot;
            controller->server_ReplicateRotUpdate(rotUpdate);
            LastSentRotation = NewRot;
        }
    }
}


void ACC_PlayerCharacter::OnRep_PlayerTID()
{
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    FPlayerTableRow* playerRow = tableSubsystem->FindTableRow<FPlayerTableRow>(TABLE_NAME::PLAYER, PlayerTID);
    if (nullptr == playerRow)
        return;

    FAbilGroupTableRow* abilGroupRow = tableSubsystem->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, playerRow->AbilGroupTID);
    if (nullptr == abilGroupRow)
        return;

    USkeletalMeshComponent* meshComp = GetMesh();

    FAnimationTableRow* AnimTable = tableSubsystem->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, FName(playerRow->AnimationTID));
    if (nullptr == AnimTable)
        return;

    meshComp->SetAnimInstanceClass(AnimTable->Animation);

    auto animInst = meshComp->GetAnimInstance();

    Anim = Cast<UAnimBase>(meshComp->GetAnimInstance());

    Anim->SetAnimType(eAnimType::ANIM_TYPE_IDLE);
    

    SetDefaultAbil(abilGroupRow->Abils);

    if (!playerRow->SkillIDs.IsEmpty())
    {
        for (auto& it : playerRow->SkillIDs)
            SetSkillData(it);
    }





    FTagTableRow* tagRow = tableSubsystem->FindTableRow<FTagTableRow>(TABLE_NAME::TAG, playerRow->TagTID);
    if (nullptr == tagRow)
        return;

    SetTagNames(tagRow->Tags);
}


//bIsSprinting 리플리케이션 처리 함수 
void ACC_PlayerCharacter::OnRep_SprintUpdate()
{
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();

    if (bIsSprinting)
    {
        bUseControllerRotationYaw = false;
        ChangeAnimation(eAnimType::ANIM_TYPE_SPRINT);

        MovementComp->MaxWalkSpeed = SprintSpeed;

        // 컨트롤러(마우스 시선) 회전 무시
        MovementComp->bUseControllerDesiredRotation = false;
        // 이동방향을 따라 캐릭터가 회전하도록 설정
        MovementComp->bOrientRotationToMovement = true;
        // 회전속도 설정
        MovementComp->RotationRate = FRotator(0.0f, SprintStartRotationRate, 0.0f);

    }
    else
    {
        bUseControllerRotationYaw = true;
        ChangeAnimation(eAnimType::ANIM_TYPE_IDLE);

        MovementComp->MaxWalkSpeed = WalkSpeed;
        MovementComp->bUseControllerDesiredRotation = true;
        MovementComp->bOrientRotationToMovement = false;
        MovementComp->RotationRate = FRotator(0.0f, SprintStopRotationRate, 0.0f);
    }
}

void ACC_PlayerCharacter::OnRep_CurrentAmmoCount()
{
    if (OnAmmoChanged.IsBound())
        OnAmmoChanged.Broadcast(CurrentAmmoCount, MaxAmmoCount);
}

void ACC_PlayerCharacter::Input_Pause(const FInputActionValue& Value)
{
    // 1. 위젯 클래스가 설정되었는지 확인
    if (PauseMenuWidgetClass.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("[Character] PauseMenuWidgetClass가 설정되지 않았습니다! BP를 확인하세요."));
        return;
    }

    // 2. 로컬 플레이어 컨트롤러 가져오기
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    // 3. UI 서브시스템 가져오기
    if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(PC->GetLocalPlayer()))
    {
        // 4. 모달 스택(최상단)에 Pause Menu 띄우기
        // FrontendGameplayTags::Frontend_WidgetStack_Modal 태그 사용
        UISubsystem->PushSoftWidgetToStackAsync(
            FrontendGameplayTags::Frontend_WidgetStack_Modal,
            PauseMenuWidgetClass,
            [](EAsyncPushWidgetState, UWidget_ActivatableBase*) {}
        );
    }
}