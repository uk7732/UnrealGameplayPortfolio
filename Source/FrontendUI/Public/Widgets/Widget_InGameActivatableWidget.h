// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonInputModeTypes.h"
#include "Widget_InGameActivatableWidget.generated.h"

class ACC_NPCCharacter;



UENUM(BlueprintType)
enum class EInGameWidgetType : uint8
{
    None,
    Inventory,  // 인벤토리
    Status,     // 상태창
    Map,        // 지도
    Skill,      // 스킬창
    ItemShop,
    Max
};

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UWidget_InGameActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    EInGameWidgetType WidgetType = EInGameWidgetType::None;

    // 2. 외부에서 데이터를 갱신하라고 명령할 때 호출할 함수 (가상 함수)
    virtual void UpdateUI();
    virtual void Init(ACC_NPCCharacter* NPC);

    virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
protected:
    TWeakObjectPtr<ACC_NPCCharacter> OwnerNPC;

    // 3. 활성화/비활성화 시점 오버라이드 (가상 함수 필수 체크!)
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;
    

private:

};
