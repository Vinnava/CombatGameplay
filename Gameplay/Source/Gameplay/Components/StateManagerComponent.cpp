// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManagerComponent.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Character/Player/PlayerBase.h"
#include "Gameplay/UI/Player/PlayerWidget.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogStateManagerComp, Log, All);

void UStateManagerComponent::SetCurrentState(FGameplayTag newState)
{
	if (newState!=currentState)
	{
		OnCharacterStateEnd.Broadcast(currentState);
		currentState = newState;
		OnCharacterStateBegin.Broadcast(currentState);

		if (const APlayerBase* playerRef = Cast<APlayerBase>(GetOwner()))
		{
			playerRef->playerWidgetRef->UpdateCurrentStateText(currentState);
		}
	}
	UE_LOG(GPLogStateManagerComp, Log, TEXT("[%s] CurrentAction = %s"), *GetName(), *currentState.ToString());
}

FGameplayTag UStateManagerComponent::GetCurrentState()
{
	return currentState;
}

void UStateManagerComponent::ResetState()
{
	SetCurrentState(FGameplayTag::EmptyTag);
}

bool UStateManagerComponent::IsCurrentStateEqualToAny(FGameplayTagContainer statesToCheck)
{
	return statesToCheck.HasTag(currentState);
}

void UStateManagerComponent::SetCurrentAction(FGameplayTag newAction)
{
	if (newAction!=currentAction)
	{
		OnCharacterActionEnd.Broadcast(currentAction);
		currentAction = newAction;
		OnCharacterActionBegin.Broadcast(currentAction);

		if (const APlayerBase* playerRef = Cast<APlayerBase>(GetOwner()))
		{
			playerRef->playerWidgetRef->UpdateCurrentActionText(currentAction);
		}
	}
	UE_LOG(GPLogStateManagerComp, Log, TEXT("[%s] CurrentAction = %s"), *GetName(), *currentAction.ToString());
}

FGameplayTag UStateManagerComponent::GetCurrentAction()
{
	return currentAction;
}

void UStateManagerComponent::ResetAction()
{
	SetCurrentAction(FGameplayTag::EmptyTag);
}

bool UStateManagerComponent::IsCurrentActionEqualToAny()
{
	return currentAction == GetCurrentAction();
}


