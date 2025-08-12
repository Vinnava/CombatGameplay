// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

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

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayButton Delegates
	playBtn->OnHovered.AddDynamic(this, &UPlayerWidget::OnHoveredPlayButton);
	playBtn->OnUnhovered.AddDynamic(this, &UPlayerWidget::OnUnhoveredPlayButton);
	playBtn->OnClicked.AddDynamic(this, &UPlayerWidget::OnClickPlayGame);

	// RestartButton Delegates
	restartBtn->OnHovered.AddDynamic(this, &UPlayerWidget::OnHoveredRestartButton);
	restartBtn->OnUnhovered.AddDynamic(this, &UPlayerWidget::OnUnhoveredRestartButton);
	restartBtn->OnClicked.AddDynamic(this, &UPlayerWidget::OnClickRestartGame);

	// QuitButton Delegates
	quitBtn->OnHovered.AddDynamic(this, &UPlayerWidget::OnHoveredQuitButton);
	quitBtn->OnUnhovered.AddDynamic(this, &UPlayerWidget::OnUnhoveredQuitButton);
	quitBtn->OnClicked.AddDynamic(this, &UPlayerWidget::OnClickQuitButton);
}

void UPlayerWidget::UpdateText(bool bIsHovered, UTextBlock* textBlockRef) const
{
	if (bIsHovered)
	{
		FSlateFontInfo hoveredFont = textBlockRef->GetFont();
		hoveredFont.Size = hoveredFontSize;
		textBlockRef->SetFont(hoveredFont);
	
		FSlateColor hoveredSlateColor = hoveredColor;
		textBlockRef->SetColorAndOpacity(hoveredSlateColor);
	}
	else
	{
		FSlateFontInfo unhoveredFont = textBlockRef->GetFont();
		unhoveredFont.Size = unhoveredFontSize;
		textBlockRef->SetFont(unhoveredFont);
	
		FSlateColor unhoveredSlateColor = unhoveredColor;
		textBlockRef->SetColorAndOpacity(unhoveredSlateColor);
	}
}

void UPlayerWidget::OnHoveredPlayButton()
{
	UpdateText(true, playBtnText);
}

void UPlayerWidget::OnUnhoveredPlayButton()
{
	UpdateText(false, playBtnText);
}

void UPlayerWidget::OnClickPlayGame()
{
	PlayAnimationReverse(playAnim, 1.0f, false);
	onClickPlayGame.Broadcast();
}

void UPlayerWidget::OnHoveredRestartButton()
{
	UpdateText(true, restartBtnText);
}

void UPlayerWidget::OnUnhoveredRestartButton()
{
	UpdateText(false, restartBtnText);
}

void UPlayerWidget::OnClickRestartGame()
{
	UKismetSystemLibrary::ExecuteConsoleCommand(this, "RestartLevel");
}

void UPlayerWidget::OnHoveredQuitButton()
{
	UpdateText(true, quitBtnText);
}

void UPlayerWidget::OnUnhoveredQuitButton()
{
	UpdateText(false, quitBtnText);
}

void UPlayerWidget::OnClickQuitButton()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UPlayerWidget::UpdateCurrentStateText(FGameplayTag newState) const
{
	textCurrentState->SetText(FText::FromString(newState.ToString()));
}

void UPlayerWidget::UpdateCurrentActionText(FGameplayTag newAction) const
{
	textCurrentAction->SetText(FText::FromString(newAction.ToString()));
}

