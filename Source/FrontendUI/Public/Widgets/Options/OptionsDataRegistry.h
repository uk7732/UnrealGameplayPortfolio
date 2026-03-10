// Vince Petrelli All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;

/**
 생성: OptionsDataRegistry가 C++ 로직인 ListDataObject들을 생성.
주입: Widget_OptionsScreen이 이 데이터 목록을 ListView에 넘김.
연결: ListView는 각 데이터 타입에 맞는 Widget_ListEntry를 생성하고, 데이터를 주입(Set).
상호작용:
유저가 위젯(View) 조작
→ 위젯이 데이터 객체(Model)의 함수 호출
→ 데이터 객체가 GameUserSettings 업데이트 (Reflection)
→ 데이터 객체가 "값 변경됨" 방송 (Delegate Broadcast)
→ 위젯이 방송을 듣고 화면 갱신 (Observer Pattern)
 */

UCLASS()
class FRONTENDUI_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	//Gets called by options screen right after the object of type UOptionsDataRegistry is created
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections;}

	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;

private:
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);	

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
