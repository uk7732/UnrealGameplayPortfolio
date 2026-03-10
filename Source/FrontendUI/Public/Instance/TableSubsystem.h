// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Table/TableDatas.h"

#include "TableSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FRONTENDUI_API UTableSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 게임 세상(World) 정보만 있으면 어디서든 이 서브시스템에 접근할 수 있게 해주는 static 함수
    UFUNCTION(BlueprintPure, Category = "Table", meta = (WorldContext = "worldContext"))
    static UTableSubsystem* Get(const UObject* worldContext);

protected:
    // 로드할 테이블 목록이 담긴 데이터 테이블
    UPROPERTY()
    TObjectPtr<UDataTable> TableLoadData;
    
    // 실제로 로드된 모든 데이터 테이블을 저장하는 맵(Map)
    UPROPERTY()
    TMap<FName, TObjectPtr<UDataTable>> Tables;

public:
    // GameInstanceSubsystem이 생성될 때 호출되는 초기화 함수
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // GameInstanceSubsystem이 소멸될 때 호출되는 함수
    virtual void Deinitialize() override;     

private:
    // 데이터 테이블들을 로드하는 실제 로직이 담긴 함수
    bool LoadTables();

public:
    // 이름으로 데이터 테이블을 찾아 반환하는 함수
    TObjectPtr<UDataTable> FindTable(FName name)
    {
        if (Tables.Contains(name))
        {
            return Tables[name];
        }
        return nullptr;
    }

    const UDataTable* GetDataTable(FName TableName) const;  


    // 이름으로 테이블을 찾고, 그 안에서 특정 행(Row)을 찾아 반환하는 템플릿 함수
    template<typename T>
    T* FindTableRow(FName tableName, FName rowName)
    {
        TObjectPtr<UDataTable> foundTable = FindTable(tableName);
        if (nullptr == foundTable)
        {
            // 찾지 못했을 때 로그를 남기면 디버깅에 유용합니다.
            UE_LOG(LogTemp, Warning, TEXT("%s 테이블을 찾을 수 없습니다."), *tableName.ToString());
            return nullptr;
        }

        // FTableRowBase를 상속받는 구조체(T)의 포인터로 행 데이터를 반환합니다.
        return foundTable->FindRow<T>(rowName, TEXT("UTableSubsystem::FindTableRow_Error"));
    }       
};