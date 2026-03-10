// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Common/GameDefines.h"
#include "SkillIndicator.generated.h"

UCLASS()
class FRONTENDUI_API ASkillIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillIndicator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(ReplicatedUsing = OnRep_DecalType)
	eDecalType NetDecalType = eDecalType::DECAL_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UDecalComponent* SkillDecalComp;

	UPROPERTY()
	UMaterialInstanceDynamic* DecalMID;

	UPROPERTY(ReplicatedUsing = OnRep_ConeAngle)
	float NetConeAngle;

	UPROPERTY(ReplicatedUsing = OnRep_DecalScale)
	FVector NetDecalScale;

	UFUNCTION()
	void OnRep_DecalScale();

	UFUNCTION()
	void OnRep_ConeAngle();

	UPROPERTY()
	TMap<eDecalType, class UMaterialInterface*> MaterialMap;
	void SetupConeAngle(float radian);
	void SetupIndicator(eDecalType Type, FVector Scale, float Duration = 0);

	UFUNCTION()
	void OnRep_DecalType();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
