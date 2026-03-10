// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MonsterDead.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UAnimNotify_MonsterDead : public UAnimNotify
{
	GENERATED_BODY()
	


	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
