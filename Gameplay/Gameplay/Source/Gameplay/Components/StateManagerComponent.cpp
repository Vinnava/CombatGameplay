// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManagerComponent.h"
#include "GameplayTagContainer.h"


void UStateManagerComponent::SetCurrentState(FGameplayTag newState)
{
	if (newState!=currentState)
	{
		OnCharacterStateEnd.Broadcast(currentState);
		currentState = newState;
		OnCharacterStateBegin.Broadcast(currentState);
	}
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
	}
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


