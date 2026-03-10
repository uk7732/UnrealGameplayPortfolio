// Vince Petrelli All Rights Reserved


#include "Animation/AnimBase.h"
#include "Characters/CC_CharacterBase.h"
#include "Component/SkillComponent.h"
#include "Characters/CC_MonsterCharacter.h"

UAnimBase::UAnimBase()
{

}

void UAnimBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UAnimBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UAnimBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UAnimBase::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void UAnimBase::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void UAnimBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UAnimBase::SetAnimType(eAnimType NewType)
{
	AnimType = NewType;
}

void UAnimBase::PlayMontage(UAnimMontage* montage, bool SkillCast)
{
	if (montage)
	{
		Montage_Play(montage);

		if (SkillCast) 
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UAnimBase::OnMontageEnded);
			Montage_SetEndDelegate(EndDelegate, montage);
		}
	}
}
void UAnimBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ACC_CharacterBase* OwnerCharacter = Cast<ACC_CharacterBase>(GetOwningActor());
	
	if (OwnerCharacter)
	{
		//몬스터 테스크 종료 
		OwnerCharacter->HandleAttackAnimationEnded(bInterrupted);
		USkillComponent* SkillComp = OwnerCharacter->FindComponentByClass<USkillComponent>();
		if (SkillComp)
			SkillComp->ClearCastSkill();
	}
}