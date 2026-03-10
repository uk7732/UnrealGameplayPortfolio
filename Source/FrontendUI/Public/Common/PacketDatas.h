// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameDefines.h"
#include "GameDatas.h"
#include "PacketDatas.generated.h"

//
// 
// 
// 
//FS_Req - From Server 서버->클라 서버가 클라에게 요청보낼때 (퀘스트 보상받기 등)
//FC_Ret - From Client 클라이언트가 서버의 요청에 대한 응답 반환값을 보냄 (스킬 캐스트 등)
//FC_Rep - From Client 클라이언트가 자신의 상태를 서버에 동기화/리플리케이트 해달라고 요청 (코인 업데이트 등)



/**
 *
 */
UCLASS()
class FRONTENDUI_API UPacketDatas : public UObject
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FS_ReqChangeAnimation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	eAnimType Type;
};

USTRUCT(BlueprintType)
struct FS_ReqSkillCastReady
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FS_ReqSkillCast
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FS_ReqSkillCastComplete
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FS_ReqQuestReward
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	eQuestType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName  MissionID;
};

USTRUCT(BlueprintType)
struct FC_RetChangeAnimation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	eAnimType Type;
};

USTRUCT(BlueprintType)
struct FC_RetSkillCastReady
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FC_RetSkillCast
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FC_RetSkillCastComplete
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TID;
};

USTRUCT(BlueprintType)
struct FC_RepRotUpdate
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator Rot;
};

USTRUCT(BlueprintType)
struct FC_RepStartSprint
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSprinting;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly)
	bool bUseControllerDesiredRotation;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly)
	bool bOrientRotationToMovement;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly)
	FRotator RotationRate;
};

USTRUCT(BlueprintType)
struct FC_RetQuestReward
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	//todo : reward data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRewardData> Rewards;
};

USTRUCT(BlueprintType)
struct FC_RepUpdateMissionDatas
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FMissionData> Data;
};

USTRUCT(BlueprintType)
struct FC_RepChangeAnimation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	eAnimType Type;
};



USTRUCT(BlueprintType)
struct FC_RepCoinUpdate
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FCoinData> Datas;

	void AddData(FCoinData& data)
	{
		FCoinData newData;
		newData.CopyFrom(data);

		Datas.Add(newData);
	}
};

USTRUCT(BlueprintType)
struct FS_ReqItemEquip
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 UID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsEquip;
};

USTRUCT(BlueprintType)
struct FC_RetItemEquip
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemEquipData Data;
};

USTRUCT(BlueprintType)
struct FS_ReqPickUpItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 ActorUID;
};

USTRUCT(BlueprintType)
struct FC_RetPickUpItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ePacketResult Result = ePacketResult::FAIL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemData Data;
};

USTRUCT(BlueprintType)
struct FC_RepItemUpdate
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemData> Datas;

	void AddData(FItemData& data)
	{
		FItemData newData;
		newData.CopyFrom(data);

		Datas.Add(newData);
	}
};

USTRUCT(BlueprintType)
struct FC_RepRewardData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRewardData> Datas;
};

