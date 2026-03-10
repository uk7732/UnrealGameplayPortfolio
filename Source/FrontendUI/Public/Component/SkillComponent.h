// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/GameDatas.h"
#include "Table/TableDatas.h"
#include "SkillComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTENDUI_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

protected:
	//내가 쓰는 스킬 총 집합 (TID, CoolTime, CostMama정보)
	UPROPERTY(Replicated)
	TArray<FSkillData> SkillDatas;

	//12345 슬롯 데이터 (Player전용)
	UPROPERTY(Replicated)
	TArray<FSkillSlotData> SkillSlots;

	//기본공격 (좌클릭)
	UPROPERTY(Replicated)
	FName NormalActionTID;

	//기본공격 (우클릭)
	UPROPERTY(Replicated)
	FName SubNormalActionTID;

	//사용중인 스킬
	UPROPERTY(Replicated)
	FName CurrentCastSkillTID;

	//사용중인 스킬의 Table정보
	FSkillTableRow CurrentSkillTableData;
	//사용중인 스킬의 데칼객체 주소
	AActor* SkillDecalActor;

	FTimerHandle CoolDownHandle;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	TArray<FSkillData> GetSkillDatas();         
	FSkillTableRow GetCurrentSkillData();		//지정 되어 있는 SkillData 반환
	FName GetCurrentSkillTID();					//지정 되어 있는 SkillTID  반환
	FName GetNormalSkillTID();					//지정 되어 있는 NormalSkillTID 반환
	FName GetSubNormalSkillTID();
	void SetCurrentSkillTID(FName tid);

	FSkillData* IsUsedSkill(FName& skillTID);  //스킬 사용 가능한지 체크 후 SkillData 정보 불러옴 쿨타임체크에도 유용
	void SetSkillData(FName& skillTID);        //사용할 스킬 지정 (일단 이건 switch문으로 수정 새로 해야함)
	FSkillData* FindSkill(FName& skillTID);    //내가 가지고 있는 스킬 찾기 SkillCoolTime 도 확인가능
						   

	void SetSkillSlot(int32 SlotNum, FName SkillTID);
	void EraseSkillSlot(int32 SlotNum);

	//SkillAction Normal은 평타캔슬 가능하도록 CurrentTID에 넣지않음
	bool SkillAction(FName skillID);			    

	void CancelCurrentSkill(); //스킬 캐스트 취소

	//데칼 관리용====
	void CreateSkillIndicator(FSkillTableRow* skilltable, bool IsPlayer);
	void EraseSkillIndicator();

	
	void RefreshSkillCoolDown(FName& skillID);  //스킬 쿨타임 초기화
	void ClearCastSkill();						//캐스팅 스킬 초기화
	void CurrentSkillResetCoolDown();			//스킬 사용 직후 스킬쿨타임 리셋	

	// 슬롯 번호로 스킬 TID를 반환하는 함수 추가
	FName GetSkillTIDInSlot(int32 SlotNum) const;

protected:
	UFUNCTION(Server, Reliable)
	void server_CurrentSkillCoolDownRogic();
	void server_CurrentSkillCoolDownRogic_Implementation();

	UFUNCTION(Server,Reliable)
	void server_SkillAction(FName SkillTID);
	void server_SkillAction_Implementation(FName SkillTID);

	UFUNCTION(Server, Reliable)
	void server_CancelCurrentSkill();
	void server_CancelCurrentSkill_Implementation();

public:
	UFUNCTION(Server, Reliable)
	void server_SetSkillData(FName TID);
	void server_SetSkillData_Implementation(FName TID);

	UFUNCTION(Server, Reliable)
	void server_SetSkillSlot(int32 slot, FName TID);
	void server_SetSkillSlot_Implementation(int32 slot, FName TID);

	UFUNCTION(Server, Reliable)
	void server_EraseSkillSlot(int32 slot);
	void server_EraseSkillSlot_Implementation(int32 slot);

	UFUNCTION(Server, Reliable)
	void server_RefeeshSkillCollDown(FName SkillTID);
	void server_RefeeshSkillCollDown_Implementation(FName SkillTID);

	UFUNCTION(Server, Reliable)
	void Server_ExecuteSkillAction();
	void Server_ExecuteSkillAction_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_RequestReload();
	void Server_RequestReload_Implementation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
