// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraOcclusionComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTENDUI_API UCameraOcclusionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UCameraOcclusionComponent();

private:
	// 플레이어와 카메라
	APawn* PlayerActor = nullptr;
	UCameraComponent* Camera = nullptr;

	// 현재/이전 투명화된 액터
	TArray<AActor*> CurrentOccludedActors;
	TArray<AActor*> PrevOccludedActors;

	TMap<UPrimitiveComponent*, UMaterialInstanceDynamic*> MIDCache;

	UPROPERTY(EditAnywhere)
	float TargetFade = 0.1f;

	// 업데이트
	void UpdateOcclusion();
	void RestorePreviousWalls();

};