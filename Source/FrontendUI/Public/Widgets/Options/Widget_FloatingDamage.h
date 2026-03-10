// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_FloatingDamage.generated.h"

/**
 * Floating Damage Widget - 데미지를 화면에 표시하는 위젯
 */
UCLASS()
class FRONTENDUI_API UWidget_FloatingDamage : public UUserWidget
{
	GENERATED_BODY()

public:
	// 위젯에 데미지 수치와 위치를 설정하는 함수 (BP에서 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Damage")
	void InitDamageInfo(float DamageAmount, bool bIsCritical, AActor* TargetActor);

protected:
	// Tick에서 타겟 위치 추적
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// C++에서 화면 좌표 변환을 도와주는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "UI")	
	void UpdateScreenPosition(AActor* TargetActor, FVector InWorldOffset);


private:
	// 추적할 타겟 액터
	UPROPERTY()
	TWeakObjectPtr<AActor> TrackedActor;

	// 월드 오프셋 (타겟 위치에서 얼마나 떨어져서 표시할지)
	UPROPERTY()
	FVector WorldOffset;

	// 생성 후 경과 시간
	float ElapsedTime = 0.0f;

public:
	// C++에서 초기화하는 함수
	void Initialize(float DamageAmount, bool bIsCritical, AActor* TargetActor);

	// 타겟 설정
	void SetTrackedActor(AActor* Actor, FVector Offset = FVector(0, 0, 100.0f));
};