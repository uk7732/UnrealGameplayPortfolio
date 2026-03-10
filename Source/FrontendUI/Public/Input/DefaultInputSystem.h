// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DefaultInputSystem.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class FRONTENDUI_API UDefaultInputSystem : public UObject
{
	GENERATED_BODY()

public:
    UDefaultInputSystem();

    // 입력 매핑 컨텍스트 에셋을 가리킬 포인터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputMappingContext> Context;

    // 이동 입력 액션 에셋을 가리킬 포인터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Move;

    // 점프 입력 액션 에셋을 가리킬 포인터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Jump;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> NormalSkillAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> SubNormalSkillAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Reload;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Sprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Dash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Skill1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Skill2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Inventory;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Interact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Potion_HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Chat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> Pause;
};
