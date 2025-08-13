// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Gameplay/Character/Player/PlayerBase.h"
#include "Kismet/KismetSystemLibrary.h"


DEFINE_LOG_CATEGORY_STATIC(GPLogPlayerWidget, Log, All);


void UPlayerWidget::PlayerWidget()
{
	
}

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate;
	timerDelegate.BindLambda([this]
	{
		PlayAnimationForward(playAnim, 1.0f, false);
	});
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, 0.5f, false);

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

void UPlayerWidget::SetHealthAndTutorialVisibility(bool bVisibility)
{
	if (bVisibility)
	{
		PlayAnimationForward(healthAnim, 1.0f, false);
		PlayAnimationReverse(toggleTabAnim, 1.0f, false);
	}
	else
	{
		PlayAnimationReverse(healthAnim, 1.0f, false);

		if (bIsTutorialVisible)
		{
			PlayAnimationReverse(toggleManualAnim, 1.0f, false);
		}
		else
		{
			PlayAnimationForward(toggleTabAnim, 1.0f, false);
		}
	}
}

void UPlayerWidget::ToggleTutorial()
{
	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate;
	float delay = toggleTutorialAnim->GetEndTime();
	
	if (bIsTutorialVisible)
	{
		if (!bCanToggleTutorial) return;
		
		bIsTutorialVisible = false;
		PlayAnimationReverse(toggleTutorialAnim, 1.0f, false);

		bCanToggleTutorial = false;
		timerDelegate.BindLambda([this]
		{
			bCanToggleTutorial = true;
		});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, delay, false);
	}
	else
	{
		if (!bCanToggleTutorial) return;
		
		bIsTutorialVisible = true;
		PlayAnimationForward(toggleTutorialAnim, 1.0f, false);

		bCanToggleTutorial = false;
		timerDelegate.BindLambda([this]
		{
			bCanToggleTutorial = true;
		});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, delay, false);
	}
}

void UPlayerWidget::ToggleMenu()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	APlayerBase* playerRef = Cast<APlayerBase>(playerController->GetPawn());
	if (!playerController || !playerRef)
	{
		UE_LOG(GPLogPlayerWidget, Warning, TEXT("[%s] [ToggleMenu] Failed: PlayerController or PlayerRef is null."), *GetClass()->GetName());
		return;
	}
	
	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate;
	float delay = restartAnim->GetEndTime();
	
	if (bIsMenuVisible)
	{
		if (!bCanToggleMenu) return;

		// Hide Menu
		bIsMenuVisible = false;
		bCanToggleMenu = false;
		SetHealthAndTutorialVisibility(true);
		PlayAnimationReverse(restartAnim, 1.0f, false);

		// Enable Player Input
		FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		playerController->SetShowMouseCursor(false);

		// Set Player default input mapping
		playerRef->SetDefaultInputMappingContext();
		
		timerDelegate.BindLambda([this]
		{
			bCanToggleMenu = true;
		});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, delay, false);
	}
	else
	{
		if (!bCanToggleMenu) return;

		// Visible Menu
		bIsMenuVisible = true;
		bCanToggleMenu = false;
		SetHealthAndTutorialVisibility(false);
		PlayAnimationForward(restartAnim, 1.0f, false);

		// Enable UI Input
		FInputModeGameAndUI inputMode;
		inputMode.SetWidgetToFocus(TakeWidget());
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		inputMode.SetHideCursorDuringCapture(true);
		playerController->SetInputMode(inputMode);
		playerController->SetShowMouseCursor(true);

		// Set Menu input mapping
		playerRef->ChangeInputMappingContext(playerRef->menuMappingContext);
		
		timerDelegate.BindLambda([this]
		{
			bCanToggleMenu = true;
		});
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, delay, false);
	}
	UE_LOG(GPLogPlayerWidget, Log, TEXT("[%s] Menu %s."), *GetClass()->GetName(), bIsMenuVisible ? TEXT("closed") : TEXT("opened"));
}
