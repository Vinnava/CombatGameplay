// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GameplayTagInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameplayTagInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAY_API IGameplayTagInterface
{
	GENERATED_BODY()

public://Functions
	virtual const FGameplayTag& GetOwnedGameplayTag() const = 0;

	virtual bool HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck) = 0;
};
