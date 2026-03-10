// Vince Petrelli All Rights Reserved


#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
	: CachedDynamicFunctionPath(InSetterOrGetterFuncPath)
{
	CachedWeakGameUserSettings = UFrontendGameUserSettings::Get();
}

// PropertyPathHelpers를 활용한 런타임 동적 바인딩
FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	FString OutStringValue;
	// CachedWeakGameUserSettings (약한 참조)객체에서 CachedDynamicFunctinoPath(동적 함수 경로)를 찾아 실행 
	PropertyPathHelpers::GetPropertyValueAsString(
		CachedWeakGameUserSettings.Get(),
		CachedDynamicFunctionPath,
		OutStringValue
	);

	return OutStringValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue)
{
	PropertyPathHelpers::SetPropertyValueFromString(
		CachedWeakGameUserSettings.Get(),
		CachedDynamicFunctionPath,
		InStringValue
	);
}