// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStateBegin, FGameplayTag, characterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStateEnd, FGameplayTag, characterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterActionBegin, FGameplayTag, characterAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterActionEnd, FGameplayTag, characterAction);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
//Functions
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
	
//Variables	
	UPROPERTY(BlueprintAssignable)
	FOnCharacterStateBegin OnCharacterStateBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterStateEnd OnCharacterStateEnd;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterActionBegin OnCharacterActionBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterActionEnd OnCharacterActionEnd;

	
protected:
//Functions
	
	
//Variables	
	UPROPERTY(VisibleDefaultsOnly)
	FGameplayTag currentState;

	UPROPERTY(VisibleDefaultsOnly)
	FGameplayTag currentAction;
	
};
