// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/CC_CharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Common/GameDatas.h"
#include "Widgets/Widget_InGameActivatableWidget.h"
#include "CC_NPCCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API ACC_NPCCharacter : public ACC_CharacterBase
{
	GENERATED_BODY()
	
public:
	ACC_NPCCharacter();

protected:
	virtual void BeginPlay() override;

public:
	// NPC 데이터 ID (테이블 조회용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_NPCTID, Category = "NPC")
	FName NPCTID;

	UFUNCTION()
	void OnRep_NPCTID();

	void SetNPCData(const FName& NPCtid);

	// 플레이어가 이 함수를 호출하여 대화를 시작함	
	virtual void Interact(AActor* Interactor) override;

protected:

	// NPC 데이터 초기화 (외형, 이름 등)
	void InitializeNPC();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	// 외부(컨트롤러)에서 이 NPC한테 "너 인벤토리 UI 내놔봐" 할 때 쓰는 함수
	TSoftClassPtr<UWidget_InGameActivatableWidget> GetWidgetClass(const FGameplayTag& Tags);

	const TArray<FMapData>& GetMapList() const;

protected:
	// NPC가 가지고 있는 위젯 목록 (에디터에서 설정 가능)
	// 예: Key=Shop, Value=BP_WeaponShopWidget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TMap<FGameplayTag, TSoftClassPtr<UWidget_InGameActivatableWidget>> NPCWidgetEntry;

	// 이 NPC가 상호작용 시 기본적으로 띄울 위젯의 태그 (테이블에서 읽어옴)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
	FGameplayTag DefaultWidgetTag;

private:
	eNPCType NPCType;

	TArray<FMapData> MyMapList;


	//npc 타입 늘어날 떄 마다 여기서 함수 늘려서 처리하기
	void MapTypeInit(class UTableSubsystem* TableSubsystem, struct FNPCTableRow* NPCRow);
	void ItemTypeInit();

	void MapInteract(AActor* Actor);
	void ItemInteract(AActor* Actor);
	//gui
};