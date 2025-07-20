// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

#include "GameplayTagContainer.h"
#include "Components/TextBlock.h"

void UPlayerWidget::UpdateCurrentStateText(FGameplayTag newState) const
{
	textCurrentState->SetText(FText::FromString(newState.ToString()));
}

void UPlayerWidget::UpdateCurrentActionText(FGameplayTag newAction) const
{
	textCurrentAction->SetText(FText::FromString(newAction.ToString()));
}
