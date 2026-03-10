// Vince Petrelli All Rights Reserved

#include "Widgets/Options/Widget_FloatingDamage.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "TimerManager.h"

void UWidget_FloatingDamage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	//// 생존 시간 초과시 제거
	//if (ElapsedTime >= LifeTime)
	//{
	//	RemoveFromParent();
	//	return;
	//}

	// 타겟이 유효하면 위치 업데이트
	if (TrackedActor.IsValid())
	{
		// 시간에 따라 Y 오프셋 증가 (위로 떠오르는 효과)
		FVector CurrentOffset = WorldOffset + FVector(0, 0, ElapsedTime * 100.0f);
		UpdateScreenPosition(TrackedActor.Get(), CurrentOffset);
	}

	ElapsedTime += InDeltaTime;
}

void UWidget_FloatingDamage::UpdateScreenPosition(AActor* TargetActor, FVector InWorldOffset)
{
	if (!IsValid(TargetActor))
	{
		RemoveFromParent();
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// 타겟의 월드 위치 + 오프셋	
	FVector WorldLoc = TargetActor->GetActorLocation() + InWorldOffset;
	FVector2D ScreenPosition;

	// 월드 좌표를 화면 좌표로 변환
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldLoc, ScreenPosition, false))
	{
		SetPositionInViewport(ScreenPosition);
	}
	else
	{
		// 화면 밖으로 나가면 제거
		RemoveFromParent();
	}
}

void UWidget_FloatingDamage::Initialize(float DamageAmount, bool bIsCritical, AActor* TargetActor)
{
	ElapsedTime = 0.0f;
	
	SetTrackedActor(TargetActor);

	// Blueprint에서 구현된 함수 호출
	InitDamageInfo(DamageAmount, bIsCritical, TargetActor);
	
	// 1.5초 후 자동 제거 타이머 설정
	FTimerHandle RemoveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		RemoveTimerHandle,
		[this]()
		{
			if (IsValid(this))
			{
				RemoveFromParent();
			}
		},
		1.5f, // 2.1초 후 제거
		false
	);
}

void UWidget_FloatingDamage::SetTrackedActor(AActor* Actor, FVector Offset)
{
	TrackedActor = Actor;
	WorldOffset = Offset;

	if (Actor)
	{
		// 초기 위치 설정
		UpdateScreenPosition(Actor, WorldOffset);
	}
}