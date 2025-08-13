// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedPlayGame);

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

private://Variables
	FColor hoveredColor {FColor::FromHex(TEXT("#D2D2D2FF"))};
	FColor unhoveredColor {FColor::FromHex(TEXT("#FFFFFFFF"))};

	bool bIsTutorialVisible {false};
	bool bIsMenuVisible {false};

	bool bCanToggleTutorial {false};
	bool bCanToggleMenu {false};
	
protected://Variables
# pragma region WidgetAnimations
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> healthAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> playAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> restartAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleTutorialAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleTabAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> toggleManualAnim;
# pragma endregion WidgetAnimations

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UHealthBarWidget> healthWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> textCurrentState;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> textCurrentAction;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> playBtn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> playBtnText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> restartBtn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> restartBtnText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> quitBtn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> quitBtnText;

	UPROPERTY(EditDefaultsOnly, Category = Initilize, meta = (ClampMin = 0, ClampMax = 50))
	float hoveredFontSize {26.0f};
	UPROPERTY(EditDefaultsOnly, Category = Initilize, meta = (ClampMin = 0, ClampMax = 50))
	float unhoveredFontSize {24.0f};

public://Variables
	UPROPERTY(BlueprintAssignable, Category = Delegate)
	FOnClickedPlayGame onClickPlayGame;

private://Functions
	void UpdateText(bool bIsHovered, UTextBlock* textBlockRef) const;
	
	UFUNCTION()
	void OnHoveredPlayButton();
	UFUNCTION()
	void OnUnhoveredPlayButton();
	UFUNCTION()
	void OnClickPlayGame();

	UFUNCTION()
	void OnHoveredRestartButton();
	UFUNCTION()
	void OnUnhoveredRestartButton();
	UFUNCTION()
	void OnClickRestartGame();

	UFUNCTION()
	void OnHoveredQuitButton();
	UFUNCTION()
	void OnUnhoveredQuitButton();
	UFUNCTION()
	void OnClickQuitButton();

protected://Functions
	virtual void NativeConstruct() override;

public://Functions
	void PlayerWidget();

	UHealthBarWidget* GetHealthBarWidget(){ return healthWidget; }

	UFUNCTION(BlueprintCallable, Category = PlayerWidget)
	void SetHealthAndTutorialVisibility(bool bVisibility);

	UFUNCTION(BlueprintCallable, Category = PlayerWidget)
	void EnableToggle() { bCanToggleTutorial = true; bCanToggleMenu = true;}
	
	void ToggleTutorial();
	void ToggleMenu();

	void UpdateCurrentStateText(struct  FGameplayTag newState) const;
	void UpdateCurrentActionText(FGameplayTag newAction) const;
};
