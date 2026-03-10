// Vince Petrelli All Rights Reserved


#include "Characters/CC_MonsterCharacter.h"
#include "Component/AbilityComponent.h"
#include "Component/SkillComponent.h"
#include "Instance/TableSubsystem.h"
#include "Table/TableDatas.h"
#include "Actor/Item/ItemBase.h"
#include "Instance/Admin4_GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Controllers/CC_AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/CC_GameStateBase.h"
#include "Camera/PlayerCameraManager.h" 
#include "Kismet/GameplayStatics.h"
#include "GameMode/WGameModeBase.h"

ACC_MonsterCharacter::ACC_MonsterCharacter()
{
    AIControllerClass = ACC_AIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    bReplicates = true;
    SetReplicateMovement(true);

    // 3. 회전 설정 (내 코드의 탑다운 설정 적용)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;

        GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);

        GetCharacterMovement()->bUseControllerDesiredRotation = false;
    }

}

void ACC_MonsterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (HasAuthority())
    {
        MoveSpeed = GetVelocity().Size();
    }

    
    // 클라이언트 거리계산 UI 계산을 할 필요가 없으므로 건너뜁니다.
    if (GetNetMode() != NM_DedicatedServer && HPBarWidget)
    {
        // 로컬 플레이어 컨트롤러 가져오기
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->PlayerCameraManager)
        {
            // 카메라 위치와 몬스터 위치 사이의 거리 계산
            FVector CameraLoc = PC->PlayerCameraManager->GetCameraLocation();
            FVector MonsterLoc = GetActorLocation();

            // 거리 제곱 계산 (DistSquared가 Sqrt 연산이 없어 성능상 유리함)
            float DistSq = FVector::DistSquared(CameraLoc, MonsterLoc);
            float VisibleDistSq = HPBarVisibleDistance * HPBarVisibleDistance;

            // 설정된 거리보다 가까우면 보이고, 멀면 숨김
            bool bShouldBeVisible = (DistSq <= VisibleDistSq);

            // 현재 상태와 다를 때만 SetVisibility 호출 (불필요한 함수 호출 방지)
            if (HPBarWidget->IsVisible() != bShouldBeVisible)
            {
                HPBarWidget->SetVisibility(bShouldBeVisible);
            }
        }
    }
}

void ACC_MonsterCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 몬스터 메쉬 가져오기
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp)
    {
        MeshComp->SetRenderCustomDepth(true);
        MeshComp->SetCustomDepthStencilValue(2); // 예: 2 = 적군 (빨간색 등)
    }

    GetWorld()->GetTimerManager().SetTimer(
        SetRotationHandle,
        this,
        &ACC_MonsterCharacter::SetRotationUpdate,
        0.1f,   // 0.1초마다 계산
        true
    );

    //if (HPBarWidget)
    //{
    //    // 일정거리 밖에서는 렌더링 중단
    //    HPBarWidget->SetCullDistance(HPBarVisibleDistance);
    //}

}

void ACC_MonsterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(SetRotationHandle);

}

void ACC_MonsterCharacter::SetSpawnRoomNum(int32 num)
{
    if (!HasAuthority())
        return;

    if (SpawnRoomNum == 0)
    {
        SpawnRoomNum = num;
        if (IsValid(GetWorld()))
        {
            AWGameModeBase* GameModeBase = Cast<AWGameModeBase>(GetWorld()->GetAuthGameMode());
            if (IsValid(GameModeBase))
            {
                GameModeBase->AddMonsterCount(num);
            }
        }
    }
}

int32 ACC_MonsterCharacter::GetSpawnRoomNum()
{
    return SpawnRoomNum;
}

ACC_CharacterBase* ACC_MonsterCharacter::GetAggroTarget()
{
    ACC_AIController* AIController = Cast<ACC_AIController>(GetController());
    if (nullptr == AIController)
        return nullptr;


    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    if (nullptr == BB)
        return nullptr;


    return Cast<ACC_CharacterBase>(BB->GetValueAsObject(TEXT("TargetActor")));
}


void ACC_MonsterCharacter::SetRotationUpdate()
{

    if (!HasAuthority()) return;

    if (bIsCastingSkill) // 스킬 시전 중인지 체크
    {
        // 스킬 시작 시점에 저장한 방향을 사용
        FVector TargetDir = SavedSkillLocation - GetActorLocation();
        FRotator TargetRot = TargetDir.Rotation();
        TargetRot.Pitch = 0.f; // 몬스터가 앞뒤로 기우는 것 방지
        TargetRot.Roll = 0.f;

        FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, 0.1f, 5.f);
        SetActorRotation(NewRot);
    }
    else
    {
        AActor* Target = GetAggroTarget();
        if (Target)
        {
            FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FRotator TargetRot = Dir.Rotation();
            TargetRot.Pitch = 0.f;
            TargetRot.Roll = 0.f;
            FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, 0.1f, 5.f); // 5.f = 회전 속도
            SetActorRotation(NewRot);
        }
    }
}

void ACC_MonsterCharacter::OnRep_MonsterTID()
{
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    FMonsterTableRow* MonsterRow = tableSubsystem->FindTableRow<FMonsterTableRow>(TABLE_NAME::MONSTER, MonsterTID);
    if (nullptr == MonsterRow)
        return;

    FAbilGroupTableRow* abilGroupRow = tableSubsystem->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, MonsterRow->AbilGroupTID);
    if (nullptr == abilGroupRow)
        return;

    FAnimationTableRow* AnimTable = tableSubsystem->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, FName(MonsterRow->AnimationTID));
    if (nullptr == AnimTable)
        return;

    USkeletalMeshComponent* meshComp = GetMesh();

    meshComp->SetAnimInstanceClass(AnimTable->Animation);

    auto animInst = meshComp->GetAnimInstance();

    Anim = Cast<UAnimBase>(meshComp->GetAnimInstance());

    Anim->SetAnimType(eAnimType::ANIM_TYPE_IDLE);


    SetDefaultAbil(abilGroupRow->Abils);

    for (auto& it : MonsterRow->SkillIDs)
        SetSkillData(it);

    FTagTableRow* tagRow = tableSubsystem->FindTableRow<FTagTableRow>(TABLE_NAME::TAG, MonsterRow->TagTID);
    if (nullptr == tagRow)
        return;

    SetTagNames(tagRow->Tags);
}

UBehaviorTree* ACC_MonsterCharacter::GetBehaviorTree()
{
    return BTAsset;
}

FSenseConfigData ACC_MonsterCharacter::GetSenseConfigData()
{
    return SenseConfigData;
}

void ACC_MonsterCharacter::OnDeathAnimationFinished()
{
    ItemDrop();



    if (HasAuthority())
    {
        if (IsValid(GetWorld()))
        {
            AWGameModeBase* GameModeBase = Cast<AWGameModeBase>(GetWorld()->GetAuthGameMode());
            if (IsValid(GameModeBase))
            {
                GameModeBase->EraseMonsterCount(SpawnRoomNum);
            }
        }


        USkillComponent* SkillComp = GetSkillComp();
        if (SkillComp != nullptr)
            SkillComp->EraseSkillIndicator();
        Destroy();
    }
}

float ACC_MonsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ResultDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (GetHP() <= 0.0f)
    {
        Die();
        return ResultDamage;
    }

    return ResultDamage;
}

float ACC_MonsterCharacter::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
    float ResultDamage = Super::TakeDamage(DamageAmount, DamageCauser);

    if (GetHP() <= 0.0f)
    {
        Die();
        return ResultDamage;
    }

    return ResultDamage;
}

void ACC_MonsterCharacter::HandleAttackAnimationEnded(bool bInterruped)
{
    OnAttackAnimationEnded.Broadcast(bInterruped);
}

void ACC_MonsterCharacter::Die()
{
    OnMonsterDeath.Broadcast(this);

    ACC_AIController* AIC = Cast<ACC_AIController>(GetController());
    if (AIC)
    {
        AIC->StopMovement();

        if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
        {
            BrainComp->StopLogic(TEXT("Monster is Dead"));
        }

        // 블랙보드 초기화 (더 이상 타겟을 쫓지 않게 함)
        // 블랙보드도 없을 수 있으니 안전하게 체크 (이미 if문이 있긴 하지만 GetBlackboardComponent() 자체도 체크 권장)
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            BB->SetValueAsObject(TEXT("TargetActor"), nullptr);
        }
    }

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
        MoveComp->SetComponentTickEnabled(false);
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
    }

    if (GetMesh())
    {
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    UAnimMontage* montage = GetSkillMontage(FName("Die"));

    if (montage)
    {
        MontageAction(montage, false);
    }
}

void ACC_MonsterCharacter::SetMonsterData(const FName& monsterTID)
{

    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    MonsterTID = monsterTID;

    if (MonsterTID.IsNone())
        return;

    FMonsterTableRow* MonsterRow = tableSubsystem->FindTableRow<FMonsterTableRow>(TABLE_NAME::MONSTER, MonsterTID);
    if (nullptr == MonsterRow)
        return;

    USkeletalMeshComponent* meshComp = GetMesh();

    FAnimationTableRow* AnimTable = tableSubsystem->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, FName(MonsterRow->AnimationTID));
    if (nullptr == AnimTable)
        return;

    meshComp->SetAnimInstanceClass(AnimTable->Animation);

    auto animInst = meshComp->GetAnimInstance();

    Anim = Cast<UAnimBase>(meshComp->GetAnimInstance());

    Anim->SetAnimType(eAnimType::ANIM_TYPE_IDLE);

    FAbilGroupTableRow* abilGroupRow = tableSubsystem->FindTableRow<FAbilGroupTableRow>(TABLE_NAME::ABILGROUP, MonsterRow->AbilGroupTID);
    if (nullptr == abilGroupRow)
        return;

    SetDefaultAbil(abilGroupRow->Abils);

    for (auto& it : MonsterRow->SkillIDs)
        SetSkillData(it);

    FTagTableRow* tagRow = tableSubsystem->FindTableRow<FTagTableRow>(TABLE_NAME::TAG, MonsterRow->TagTID);
    if (nullptr == tagRow)
        return;

    SetTagNames(tagRow->Tags);

}

float ACC_MonsterCharacter::GetMoveSpeed()
{
    return MoveSpeed;
}

//void ACC_MonsterCharacter::ItemDrop()
//{
//    // 서버가 아니거나, 드롭할 아이템 목록이 없으면 실행하지 않습니다.
//    if (!HasAuthority() || ItemDropList.Num() == 0)
//        return;
//
//    UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
//    UAdmin4_GameInstance* GameInstance = GetGameInstance<UAdmin4_GameInstance>();    
//    if (nullptr == Tablesystem)
//        return;
//
//
//    UAdmin4_GameInstance* GameInstance = Cast<UAdmin4_GameInstance>(GetGameInstance());
//    if (nullptr == GameInstance)
//        return;
//
//    for (auto& it : ItemDropList)
//    {
//        float Droppedchance = FMath::FRandRange(0.f, 100.f);
//
//        if (it.ItemDropchance >= Droppedchance)
//        {
//            FItemTableRow* ItemTable = Tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, it.ITemTID);
//            if (nullptr == ItemTable)
//                return;
//
//            int32 ItemDropCount = 1;
//
//            if (ItemTable->Type == eItemType::COMPONENT || ItemTable->Type == eItemType::CONSUME)
//            {
//                int32 maxCountchance = it.MaxDropCount;
//                ItemDropCount = FMath::RandRange(1, maxCountchance);
//            }
//
//            FVector BaseLocation = GetActorLocation();
//
//            //  랜덤 오프셋 계산 (예: 반지름 50cm 내에서 랜덤 분산)
//            const float DropRadius = 50.0f; // 50.0f = 50cm
//            float RandomX = FMath::FRandRange(-DropRadius, DropRadius);
//            float RandomY = FMath::FRandRange(-DropRadius, DropRadius);
//
//
//            FVector Offset = FVector(RandomX, RandomY, 0.0f);
//
//            //  최종 스폰 위치 계산
//            FVector DropLocation = BaseLocation + Offset;
//
//            AItemBase* Item = GetWorld()->SpawnActor<AItemBase>(AItemBase::StaticClass(), DropLocation, FRotator::ZeroRotator);
//            Item->InitializedItemData(it.ITemTID, GameInstance->GetNextItemUID_Server(), ItemDropCount);
//        }
//    }
//}

void ACC_MonsterCharacter::ItemDrop()
{
    // 서버가 아니거나, 드롭할 아이템 목록이 없으면 실행하지 않습니다.
    if (!HasAuthority() || ItemDropList.Num() == 0)
        return;

    UTableSubsystem* Tablesystem = UTableSubsystem::Get(this);
    UAdmin4_GameInstance* GameInstance = GetGameInstance<UAdmin4_GameInstance>();
    if (!Tablesystem || !GameInstance) return;


    // 드롭 리스트를 순회하며 각 아이템을 드롭할지 결정합니다.
    for (const FItemDropData& DropData : ItemDropList)
    {
        // 0.0 ~ 100.0 사이의 랜덤 숫자를 뽑습니다.
        float RandomChance = FMath::FRandRange(0.f, 100.f);

        // 랜덤 숫자가 아이템의 드롭 확률보다 낮으면 드롭 성공!
        if (RandomChance <= DropData.ItemDropchance)
        {
            FItemTableRow* ItemTable = Tablesystem->FindTableRow<FItemTableRow>(TABLE_NAME::ITEM, DropData.ITemTID);
            //  테이블 정보나 ItemClass가 유효하지 않으면 이 아이템은 건너뜁니다.
            if (!ItemTable || !ItemTable->ItemClass) continue;

            // 스택 가능 아이템이면 1 ~ MaxDropCount 사이의 랜덤 개수, 아니면 1개

            int32 DropCount = 1;
            if (ItemTable->MaxStackCount > 1)
            {
                DropCount = FMath::RandRange(1, DropData.MaxDropCount);
            }

            // 몬스터 주변 랜덤 위치에 드롭
            FVector SpawnLocation = GetActorLocation() + FVector(FMath::FRandRange(-50.f, 50.f), FMath::FRandRange(-50.f, 50.f), 20.f);
            SpawnLocation.Z = 30.f;

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // AItemBase::StaticClass() 대신 테이블에 정의된 ItemClass를 사용합니다.
            AItemBase* SpawnedItem = GetWorld()->SpawnActor<AItemBase>(ItemTable->ItemClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

            if (SpawnedItem)
            {
                // 생성된 아이템에 정보(TID, 새로운 UID, 개수)를 초기화해줍니다.
                SpawnedItem->InitializedItemData(DropData.ITemTID, GameInstance->GetNextItemUID_Server(), DropCount);

            }
        }
    }
}

void ACC_MonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACC_MonsterCharacter, MonsterTID);
    DOREPLIFETIME(ACC_MonsterCharacter, MoveSpeed);
    DOREPLIFETIME(ACC_MonsterCharacter, SpawnRoomNum);
}

