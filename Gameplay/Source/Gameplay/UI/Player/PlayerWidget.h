// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

private://Variables

	
protected://Variables
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> textCurrentState;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> textCurrentAction;

public://Variables
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UHealthBarWidget> healthWidget;

private://Functions


protected://Functions

	
public://Functions
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void RestartWidget();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void ToggleTutorial();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void RemoveRestartWidget();

	void UpdateCurrentStateText(struct  FGameplayTag newState) const;
	void UpdateCurrentActionText(FGameplayTag newAction) const;
};
