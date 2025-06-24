// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnToggleCombat, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
private://Variables
	TObjectPtr<class ABaseWeapon> mainWeapon;

public://Variables
	int32 attackCount;
	FOnToggleCombat onToggleCombat;
	bool bIsAttackSaved;
	bool bIsCombatEnabled;
	bool bCanContinueAttack;

public://Functions
	void SetMainWeapon(ABaseWeapon* inMainWeapon);
	ABaseWeapon* GetMainWeapon() const;
	void SetCombatEnabled(bool bIsEnabled);
	bool IsCombatEnabled() const { return bIsCombatEnabled; }
	void ResetCombat();
};
