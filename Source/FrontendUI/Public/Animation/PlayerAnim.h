// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimBase.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UPlayerAnim : public UAnimBase
{
	GENERATED_BODY()

public:
	UPlayerAnim();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;
	virtual void NativeUninitializeAnimation() override;
	virtual void NativeBeginPlay() override;


};
