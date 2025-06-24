// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StateManagerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStateBegin, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStateEnd, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterActionBegin, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterActionEnd, FGameplayTag);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()
private://Variables
	FGameplayTag currentState;
	FGameplayTag currentAction;
	
public://Variables
	FOnCharacterStateBegin OnCharacterStateBegin;
	
	FOnCharacterStateEnd OnCharacterStateEnd;
	
	FOnCharacterActionBegin OnCharacterActionBegin;
	
	FOnCharacterActionEnd OnCharacterActionEnd;

public://Functions
	UFUNCTION(BlueprintCallable)
	void SetCurrentState(FGameplayTag newState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetCurrentState();

	UFUNCTION(BlueprintCallable)
	void ResetState();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCurrentStateEqualToAny(FGameplayTagContainer statesToCheck);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentAction(FGameplayTag newAction);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetCurrentAction();

	UFUNCTION(BlueprintCallable)
	void ResetAction();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCurrentActionEqualToAny();
};
