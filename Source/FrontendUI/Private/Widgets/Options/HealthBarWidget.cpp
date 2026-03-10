// Vince Petrelli All Rights Reserved


#include "Widgets/Options/HealthBarWidget.h"

void UHealthBarWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (PB_Health)
	{
		// 0으로 나누기 방지 (Safe Divide)
		if (MaxHP > 0.0f)
		{
			float Percent = CurrentHP / MaxHP;
			PB_Health->SetPercent(Percent);
		}
		else
		{
			PB_Health->SetPercent(0.0f);
		}
	}
}