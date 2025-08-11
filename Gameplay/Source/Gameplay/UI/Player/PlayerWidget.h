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
# pragma region WidgetAnimations
	
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> healthAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> playAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> restartAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleTutorialAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleTabAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleManualAnim;
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> hideAllOverlayAnim;
# pragma endregion WidgetAnimations
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> textCurrentState;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> textCurrentAction;

public://Variables
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UHealthBarWidget> healthWidget;

private://Functions


protected://Functions

	
public://Functions
	void PlayerWidget();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void RestartWidget();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void ToggleTutorial();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerWidget")
	void RemoveRestartWidget();

	void UpdateCurrentStateText(struct  FGameplayTag newState) const;
	void UpdateCurrentActionText(FGameplayTag newAction) const;
};
