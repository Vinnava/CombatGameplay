// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagData.generated.h"


#pragma region ActionTag

const FGameplayTag lightAttackActionTag = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Attack.LightAttack"));
const FGameplayTag dodgeActionTag       = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Dodge"));
const FGameplayTag dieActionTag         = FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Die"));
#pragma endregion ActionTag 


#pragma region StateTag

const FGameplayTag deadStateTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dead"));
const FGameplayTag attackingStateTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Attacking"));
const FGameplayTag disabledStateTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Disabled"));
const FGameplayTag dodgingStateTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dodging"));
const FGameplayTag generalActionStateTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.GeneralAction"));
#pragma endregion StateTag


UCLASS()
class GAMEPLAY_API UGameplayTagData : public UObject
{
	GENERATED_BODY()
	
};
