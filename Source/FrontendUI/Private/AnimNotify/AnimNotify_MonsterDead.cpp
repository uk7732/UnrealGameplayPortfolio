// Vince Petrelli All Rights Reserved


#include "AnimNotify/AnimNotify_MonsterDead.h"
#include "Characters/CC_MonsterCharacter.h"

void UAnimNotify_MonsterDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ACC_MonsterCharacter* Monster = Cast<ACC_MonsterCharacter>(MeshComp->GetOwner());
	
	if (Monster)
		Monster->OnDeathAnimationFinished();
}
