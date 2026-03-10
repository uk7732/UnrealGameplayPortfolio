// Vince Petrelli All Rights Reserved


#include "Instance/TableSubsystem.h"
#include "Kismet/GameplayStatics.h" 

// 데이터 테이블을 경로로부터 로드하는 매크로
#define TABLE_LOAD(path) Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *path))

void UTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 서브시스템이 초기화될 때 모든 테이블을 로드합니다.
    if (false == LoadTables())
    {
        UE_LOG(LogTemp, Error, TEXT("데이터 테이블 로드에 실패했습니다!"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("데이터 테이블 로드 성공."));
    }
}

void UTableSubsystem::Deinitialize()
{
    Super::Deinitialize();
    // 테이블 맵을 비웁니다.
    Tables.Empty();
}

UTableSubsystem* UTableSubsystem::Get(const UObject* worldContext)
{
    if (nullptr == worldContext) return nullptr;

    UGameInstance* inst = UGameplayStatics::GetGameInstance(worldContext);
    if (nullptr == inst) return nullptr;

    return inst->GetSubsystem<UTableSubsystem>();
}

bool UTableSubsystem::LoadTables()
{
    // 1. 가장 먼저, 어떤 테이블들을 로드할지 알려주는 "TableLoad" 테이블을 먼저 로드합니다.
    // 이 경로의 에셋은 나중에 에디터에서 직접 만들어야 합니다.
    const FString tableLoadPath = TEXT("/Game/TableData/TableLoad");
    TableLoadData = TABLE_LOAD(tableLoadPath);

    if (nullptr == TableLoadData)
    {
        UE_LOG(LogTemp, Error, TEXT("DT_TableLoad를 찾을 수 없습니다. 경로: %s"), *tableLoadPath);
        return false;
    }

    bool bResult = true;
    // 2. "TableLoad" 테이블의 모든 행을 순회합니다.
    TableLoadData->ForeachRow<FTableLoadRow>(TEXT("UTableSubsystem::LoadTables"), [this, &bResult](const FName& key, const FTableLoadRow& value)
        {
            // 3. IsLoad가 true로 체크된 행만 처리합니다.
            if (value.IsLoad)
            {
                // 4. 해당 행의 TablePath로부터 데이터 테이블을 로드합니다.
                TObjectPtr<UDataTable> loadTable = TABLE_LOAD(value.TablePath);
                if (IsValid(loadTable))
                {
                    // 5. 성공적으로 로드되면, Tables 맵에 키(key)와 함께 저장합니다.
                    Tables.Add(key, loadTable);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("%s 테이블 로드 실패! 경로: %s"), *key.ToString(), *value.TablePath);
                    bResult = false;
                }
            }
        });

    return bResult;
}

const UDataTable* UTableSubsystem::GetDataTable(FName TableName) const
{
    // Tables 맵에 해당 이름의 테이블이 있는지 확인
    if (Tables.Contains(TableName))
    {
        // TObjectPtr은 포인터로 자동 변환됩니다.
        return Tables[TableName];
    }

    UE_LOG(LogTemp, Warning, TEXT("GetDataTable 실패: [%s] 테이블을 찾을 수 없습니다."), *TableName.ToString());
    return nullptr;
}