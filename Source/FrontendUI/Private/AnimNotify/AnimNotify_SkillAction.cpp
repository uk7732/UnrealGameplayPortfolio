// Vince Petrelli All Rights Reserved

#include "AnimNotify/AnimNotify_SkillAction.h"
#include "Characters/CC_CharacterBase.h"
#include "Table/TableDatas.h"
#include "Component/SkillComponent.h"
#include "Instance/TableSubsystem.h"
#include "SkillAction/SkillActionBase.h"


void UAnimNotify_SkillAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

	ACC_CharacterBase* character = Cast<ACC_CharacterBase>(MeshComp->GetOwner());

	if (character)
	{
        USkillComponent* SkillComp = character->GetSkillComp();
        if (SkillComp)
        {
            SkillComp->EraseSkillIndicator();

            if(character->IsLocallyControlled())
                SkillComp->Server_ExecuteSkillAction();
        }

    }
}