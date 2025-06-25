// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

struct FPerformDeath
{
	TArray<TObjectPtr<AActor>> actorsToDestory;
	float duration;
};

struct FPerformAction
{
	bool bSuccess;
	float actionDuration;
};

struct FPerformAttack
{
	bool bSuccess;
	float attackDuration;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAY_API ICombatInterface
{
	GENERATED_BODY()

public://Functions
	virtual void ContinueAttack() = 0;
	virtual void ResetCombat() = 0;
	virtual bool CanReciveDamage() = 0;
	virtual FPerformDeath PerformDeath() = 0;
	virtual bool PerformHitReaction(FVector hitLocation, float damage) = 0;
	virtual FPerformAction PerformAction(FGameplayTag characterState,
										FGameplayTag characterAction,
										int32 montageIndex,
										bool randomIndex) = 0;
	virtual FPerformAttack PerformAttack(FGameplayTag attackType,
										int32 attackIndex,
										bool bRandomIndex,
										bool bIsCalledByAI,
										float playRate) = 0;
};
