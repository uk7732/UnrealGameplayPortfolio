// Vince Petrelli All Rights Reserved


#include "Characters/CC_NPCCharacter.h"
#include "Instance/TableSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Table/TableDatas.h" // FNPC_TableRow 같은 구조체가 있다고 가정
#include "Controllers/CC_AIController.h"      // AI 컨트롤러 헤더
#include "BehaviorTree/BehaviorTree.h"        // BT 헤더
#include "Instance/Admin4_GameInstance.h"     // 게임 인스턴스 (레벨 이동용)
#include "Controllers/CC_PlayerController.h" 
#include "CommonActivatableWidget.h" 



ACC_NPCCharacter::ACC_NPCCharacter()
{
    // NPC는 보통 AI가 제어하거나 제자리에서 대기함
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    GetMesh()->SetCustomDepthStencilValue(1);
}

void ACC_NPCCharacter::BeginPlay()
{
    Super::BeginPlay();
}


void ACC_NPCCharacter::MapTypeInit(UTableSubsystem* TableSubsystem, FNPCTableRow* NPCRow)
{
    FName MapGroupTID = NPCRow->NPCTIDDatas.MapGroupTID;

    FMapGroupTableRow* MapGroupRow = TableSubsystem->FindTableRow<FMapGroupTableRow>(TABLE_NAME::MAPGROUP, MapGroupTID);
    if (MapGroupRow)
    {
        for (auto& it : MapGroupRow->MapGroupList)
        {
            FName MapTID = it.MapTID;
            FMapTableRow* MapTableRow = TableSubsystem->FindTableRow<FMapTableRow>(TABLE_NAME::MAP, MapTID);

            FMapData MapData = {};
            MapData.MapTID = it.MapTID;
            MapData.MapNumber = it.MapNumber;
            MapData.DisplayName = MapTableRow->DisplayName;
            MapData.Description = MapTableRow->Description;
            MapData.MapImage = MapTableRow->MapImage;
            MyMapList.Add(MapData);
        }
        MyMapList.Sort();
    }
    //Map타입 init로직 추가 작성
}

void ACC_NPCCharacter::ItemTypeInit()
{
    //아이템타입 init 로직 작성
}

void ACC_NPCCharacter::MapInteract(AActor* Actor)
{
    // 1. 레벨 이동이 설정된 NPC라면 이동 처리
//if (!LevelNameKey.IsNone())
//{
//    UE_LOG(LogTemp, Warning, TEXT("NPC [%s]: 플레이어에게 [%s] 이동 제안 중..."), *GetName(), *LevelNameKey.ToString());

//    // 상호작용한 대상이 폰(캐릭터)인지 확인
//    if (APawn* PlayerPawn = Cast<APawn>(Interactor))
//    {
//        // 해당 캐릭터의 컨트롤러 가져오기
//        if (ACC_PlayerController* PC = Cast<ACC_PlayerController>(PlayerPawn->GetController()))
//        {
//            // [중요] 바로 이동(GameInst->HostMultiplayer)하지 않고,
//            // 플레이어에게 팝업을 띄우라고(Client RPC) 명령만 내립니다.
//            PC->Client_ShowMapSelectionScreen();                
//            PC->Client_ShowLevelTransferConfirm(LevelNameKey);
//        }
//    }
//    return;

        //}

    // 2. 그 외 일반 대화 NPC 처리
   /* UE_LOG(LogTemp, Log, TEXT("NPC %s: 안녕하세요!"), *GetName());*/
    // TODO: 클라이언트에게 UI 띄우라고 RPC 보내기
}

void ACC_NPCCharacter::ItemInteract(AActor* Actor)
{

}

void ACC_NPCCharacter::OnRep_NPCTID()
{
    SetNPCData(NPCTID);
}

void ACC_NPCCharacter::SetNPCData(const FName& NPCtid)
{
    UTableSubsystem* tableSubsystem = UTableSubsystem::Get(this);
    if (nullptr == tableSubsystem)
        return;

    NPCTID = NPCtid;

    if (NPCTID.IsNone())
        return;

    FNPCTableRow* NPCRow = tableSubsystem->FindTableRow<FNPCTableRow>(TABLE_NAME::NPC, NPCTID);
    if (nullptr == NPCRow)
        return;

    USkeletalMeshComponent* meshComp = GetMesh();

    FAnimationTableRow* AnimTable = tableSubsystem->FindTableRow<FAnimationTableRow>(TABLE_NAME::ANIMATION, FName(NPCRow->AnimationTID));
    if (nullptr == AnimTable)
        return;

    meshComp->SetAnimInstanceClass(AnimTable->Animation);

    auto animInst = meshComp->GetAnimInstance();
    
    Anim = Cast<UAnimBase>(meshComp->GetAnimInstance());

    if(Anim)
        Anim->SetAnimType(eAnimType::ANIM_TYPE_IDLE);

    FTagTableRow* tagRow = tableSubsystem->FindTableRow<FTagTableRow>(TABLE_NAME::TAG, NPCRow->TagTID);
    if (nullptr == tagRow)
        return;

    SetTagNames(tagRow->Tags);

    if (HasAuthority())
    {
        if (!NPCRow->AIBehavior.IsNull())
        {
            ACC_AIController* AICon = Cast<ACC_AIController>(GetController());
            if (AICon)
            {
                // SoftObjectPtr을 로드하여 컨트롤러에게 전달
                UBehaviorTree* BT = NPCRow->AIBehavior.LoadSynchronous();
                AICon->RunAI(BT);
            }
        }
    }

    for (auto& it : NPCRow->WidgetEntry)
    {
        NPCWidgetEntry.Add(it.WidgetName, it.Widget_Activatable);
    }

    if (NPCRow->StartWidgetTag.IsValid())
    {
        DefaultWidgetTag = NPCRow->StartWidgetTag;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("NPC %s: DefaultWidgetTag 설정 안됨"), *GetName());
    }

    NPCType = NPCRow->Type;

    switch (NPCType)
    {
    case eNPCType::NPC_MAP :
    {
        MapTypeInit(tableSubsystem, NPCRow);
        break;
    }
    case eNPCType::NPC_ITEM :
        break;
    }

}

void ACC_NPCCharacter::InitializeNPC()
{
    if (NPCTID.IsNone()) return;

    UTableSubsystem* TableSub = UTableSubsystem::Get(this);
    if (!TableSub) return;

    // 데이터 테이블에서 행 정보 가져오기 (TABLE_NAME::NPC는 상수로 정의되어 있다고 가정)
    FNPCTableRow* Row = TableSub->FindTableRow<FNPCTableRow>(TABLE_NAME::NPC, NPCTID);
    

    //리팩토링하기
    
    //if (Row)
    //{
    //    // 1. 외형(Mesh) 설정
    //    if (Row->Mesh)
    //    {
    //        GetMesh()->SetSkeletalMesh(Row->Mesh);
    //    }

    //    // 2. 애니메이션 설정
    //    if (!Row->AnimBP.IsNull())
    //    {
    //        GetMesh()->SetAnimInstanceClass(Row->AnimBP.LoadSynchronous());
    //    }

    //    // 3. [레벨 이동] 데이터 저장
    //    LevelNameKey = Row->TargetLevelName;

    //    // 4. [AI 실행] 서버인 경우에만 AI 가동
    //    if (HasAuthority() && !Row->AIBehavior.IsNull())
    //    {
    //        ACC_AIController* AICon = Cast<ACC_AIController>(GetController());
    //        if (AICon)
    //        {
    //            // SoftObjectPtr을 로드하여 컨트롤러에게 전달
    //            UBehaviorTree* BT = Row->AIBehavior.LoadSynchronous();
    //            AICon->RunAI(BT);
    //        }
    //    }
    //}
}

void ACC_NPCCharacter::Interact(AActor* Interactor)
{
    // 이 함수는 PlayerCharacter의 Server RPC를 통해 호출되므로 '서버'에서 실행됩니다.
    ACC_PlayerCharacter* Player = Cast<ACC_PlayerCharacter>(Interactor);
    if (nullptr == Player)
        return;

    ACC_PlayerController* PC = Cast<ACC_PlayerController>(Player->GetController());
    if (nullptr == PC)
        return;

    FGameplayTag TagToSend = DefaultWidgetTag;
    if (!TagToSend.IsValid())
        return;


    switch (NPCType)
    {
    case eNPCType::NPC_MAP :
        // 맵 이동 관련 특수 로직이 있다면 여기서 처리 (예: 데이터 세팅)
        MapInteract(Interactor);
        // [중요] 맵 선택창도 UI이므로, 결국 클라이언트에게 띄우라고 명령해야 함
        // 기존 코드: PC->Client_ShowMapSelectionScreen(); 
        // 리팩토링 코드:
        PC->Client_OnInteractWithNPC(this, TagToSend);
        break;
    case eNPCType::NPC_ITEM :
        ItemInteract(Interactor);
        PC->Client_OnInteractWithNPC(this, TagToSend);
        break;

    default:
        //기본 대화형 NPC
        PC->Client_OnInteractWithNPC(this, TagToSend);
        break;
    }




}


void ACC_NPCCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACC_NPCCharacter, NPCTID);
}

TSoftClassPtr<UWidget_InGameActivatableWidget> ACC_NPCCharacter::GetWidgetClass(const FGameplayTag& Tag)
{
    return NPCWidgetEntry[Tag];
}

const TArray<FMapData>& ACC_NPCCharacter::GetMapList() const
{
    return MyMapList;
}


