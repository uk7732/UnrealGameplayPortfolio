// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Common/GameDefines.h"
#include "AnimBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FRONTENDUI_API UAnimBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAnimBase();



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	eAnimType AnimType;

protected:
	//1. 메인 스레드 : NativeUpdateAnimation(변수 업데이트)
	//2. 워커 스레드 : NativeThreadSafeUpdateAnimation(애니메이션 스레드 안전 계산)
	//3. (엔진 내부) 애니메이션 그래프 평가(애니메이션 그래프 계산)
	//4. 메인 스레드 : NativePostEvaluateAnimation(최종 결과 후처리)


	virtual void NativeInitializeAnimation() override;                          //애니매이션 인스턴스 초기화(주로 로드,생성될 때) 시 호출, 애니매이션 블루프린트에서 변수 초기화 등에 사용
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;            //매 프레임마다 호출, DeltaSeconds는 지난 프레임과 현재 프레임 사이의 시간 간격
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;  //멀티스레딩 환경에서 안전하게 애니메이션 업데이트를 수행할 때 호출
	virtual void NativePostEvaluateAnimation() override;					    //애니메이션 평가 후 호출, 애니메이션 블루프린트에서 최종 애니메이션 데이터 처리 등에 사용
	virtual void NativeUninitializeAnimation() override;						//애니메이션 인스턴스가 파괴되거나 더이상 사용되지 않을 때 호출, 리소스 해제 등에 사용
	virtual void NativeBeginPlay() override;									//애니매이션 인스턴스 생성 후 한번 호출 게임플레이에 필요한 초기설정 수행하는 데 사용
	
public:
	UFUNCTION()
	void SetAnimType(eAnimType NewType);
	UFUNCTION()
	eAnimType GetAnimType() { return AnimType; }

	//SkillCast쪽 true = OnMontageEnded 실행(스킬초기화) false = 바인딩 함수 실행 안함
	void PlayMontage(UAnimMontage* montage, bool SkillCast);
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);


};
