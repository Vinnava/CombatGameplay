// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

#include "GameplayTagContainer.h"
#include "Components/TextBlock.h"

void UPlayerWidget::PlayerWidget()
{
	PlayAnimationForward(hideAllOverlayAnim, 1.0f, false);

	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate;
	timerDelegate.BindLambda([this]
	{
		PlayAnimationForward(playAnim, 1.0f, false);
	});
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, 0.5f, false);
}

void UPlayerWidget::UpdateCurrentStateText(FGameplayTag newState) const
{
	textCurrentState->SetText(FText::FromString(newState.ToString()));
}

void UPlayerWidget::UpdateCurrentActionText(FGameplayTag newAction) const
{
	textCurrentAction->SetText(FText::FromString(newAction.ToString()));
}
