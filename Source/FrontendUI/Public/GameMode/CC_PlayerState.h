// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Common/GameDatas.h"
#include "Net/UnrealNetwork.h"
#include "CC_PlayerSTate.generated.h"




class UInventoryComponent;
class UEquipmentComponent;
/**
 * 
 */
UCLASS()
class FRONTENDUI_API ACC_PlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	ACC_PlayerState();



	UInventoryComponent* GetInventory();
	UEquipmentComponent* GetEquipment();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<UInventoryComponent > Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<UEquipmentComponent> Equipment;
private:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
