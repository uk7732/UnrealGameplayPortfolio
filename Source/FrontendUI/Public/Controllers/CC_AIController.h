// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CC_AIController.generated.h"


/**
 *
 */
UCLASS()
class FRONTENDUI_API ACC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ACC_AIController();

	// 외부(NPC)에서 AI 실행을 요청하는 함수
	void RunAI(UBehaviorTree* InBTAsset);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBlackboardData* BBAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BTAsset;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp;

	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TArray<class ACC_PlayerCharacter*> PlayerActors;


	UFUNCTION()
	void OnTargetDetected(AActor* actor, struct FAIStimulus Stimulus);
	virtual void PostInitializeComponents() override;
public:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	AActor* GetNearestDetectedActor();
	
};
