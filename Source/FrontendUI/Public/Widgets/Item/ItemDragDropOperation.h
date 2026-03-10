// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Common/GameDefines.h"

#include "ItemDragDropOperation.generated.h"

// 드래그가 시작된 위치를 구분하기 위한 Enum
UENUM(BlueprintType)
enum class EDragSourceType : uint8
{
	Inventory,  // 인벤토리에서 시작
	Equipment   // 장비창에서 시작
};

UCLASS()
class FRONTENDUI_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()


public:
    // 드래그 중인 아이템의 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    int64 ItemUID;

    // 인벤토리 몇 번째 칸에서 왔는지 (Swap용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    int32 SourceIndex;

    // 어디서 드래그를 시작했는지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    EDragSourceType SourceType;

    // (장비창에서 왔을 경우) 장비 슬롯 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    eItemEquipType SourceEquipType;

    // 아이템 테이블 ID (버릴 때 필요)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    FName ItemTID;
};
