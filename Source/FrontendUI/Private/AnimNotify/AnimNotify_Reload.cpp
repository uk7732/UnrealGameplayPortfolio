// Vince Petrelli All Rights Reserved

#include "AnimNotify/AnimNotify_Reload.h"
#include "Characters/CC_CharacterBase.h"
#include "Table/TableDatas.h"
#include "Component/SkillComponent.h"
#include "SkillAction/SkillActionBase.h"


void UAnimNotify_Reload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ACC_PlayerCharacter* character = Cast<ACC_PlayerCharacter>(MeshComp->GetOwner());

	if (character)
	{
		USkillComponent* SkillComp = character->GetSkillComp();
		if (SkillComp)
		{
			SkillComp->Server_RequestReload();
		}

	}
}
