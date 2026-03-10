// Vince Petrelli All Rights Reserved


#include "Input/DefaultInputSystem.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"


UDefaultInputSystem::UDefaultInputSystem()
{
    // 템플릿의 기본 IMC 에셋 경로
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> context(TEXT("/Game/Input/IMC_Default.IMC_Default"));
        Context = context.Object;

    // 템플릿의 기본 IA 에셋 경로들
    static ConstructorHelpers::FObjectFinder<UInputAction> moveAction(TEXT("/Game/Input/MovementActions/IA_Move.IA_Move"));
    if (moveAction.Succeeded())
        Move = moveAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> jumpAction(TEXT("/Game/Input/MovementActions/IA_Jump.IA_Jump"));
    if (jumpAction.Succeeded())
        Jump = jumpAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> NormalAction(TEXT("/Game/Input/IA_NormalSkillAction.IA_NormalSkillAction"));
    if (NormalAction.Succeeded())
        NormalSkillAction = NormalAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> SubNormalAction(TEXT("/Game/Input/IA_SubNormalSkillAction.IA_SubNormalSkillAction"));
    if (SubNormalAction.Succeeded())
        SubNormalSkillAction = SubNormalAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> ReloadAction(TEXT("/Game/Input/IA_Reload.IA_Reload"));
    if (ReloadAction.Succeeded())
        Reload = ReloadAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> SprintAction(TEXT("/Game/Input/MovementActions/IA_Sprint.IA_Sprint"));
    if (SprintAction.Succeeded())
        Sprint = SprintAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> DashAction(TEXT("/Game/Input/IA_Dash.IA_Dash"));
    if (DashAction.Succeeded())
        Dash = DashAction.Object;


    static ConstructorHelpers::FObjectFinder<UInputAction> Skill1Action(TEXT("/Game/Input/IA_Skill1.IA_Skill1"));
    if (Skill1Action.Succeeded())
        Skill1 = Skill1Action.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> Skill2Action(TEXT("/Game/Input/IA_Skill2.IA_Skill2"));
    if (Skill2Action.Succeeded())
        Skill2 = Skill2Action.Object;
    
    static ConstructorHelpers::FObjectFinder<UInputAction> InventoryAction(TEXT("/Game/Input/IA_Inventory.IA_Inventory"));
    if (InventoryAction.Succeeded())
        Inventory = InventoryAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> InteractAction(TEXT("/Game/Input/IA_Interact.IA_Interact"));
    if (InteractAction.Succeeded())
        Interact= InteractAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> PotionAction(TEXT("/Game/Input/IA_Potion_HP.IA_Potion_HP"));
    if (PotionAction.Succeeded())
        Potion_HP = PotionAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> ChatAction(TEXT("/Game/Input/IA_Chat.IA_Chat"));
    if (ChatAction.Succeeded())
        Chat = ChatAction.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> PauseAction(TEXT("/Game/Input/IA_Pause.IA_Pause"));
    if (PauseAction.Succeeded())
        Pause = PauseAction.Object;
}
