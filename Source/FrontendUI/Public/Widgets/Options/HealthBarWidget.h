// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h" 

#include "HealthBarWidget.generated.h"

UCLASS()
class FRONTENDUI_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 이 함수를 델리게이트에 연결할 것입니다.
	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);

protected:
	// [중요] meta = (BindWidget)을 쓰면 블루프린트의 같은 이름의 위젯과 자동 연결됩니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;
};
