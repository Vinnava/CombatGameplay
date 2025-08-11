// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::UpdateHealth(float health) const
{
	float healthProgress = health / 100.0f;
	HealthBar->SetPercent(healthProgress);

	const FColor greenRGB = FColor::FromHex(TEXT("#27B300FF"));
	FLinearColor color = FLinearColor::FromSRGBColor(greenRGB);
	if (healthProgress < 0.3f)
	{
		color = FColor::Red;
	}
	HealthBar->SetFillColorAndOpacity(color);
}
