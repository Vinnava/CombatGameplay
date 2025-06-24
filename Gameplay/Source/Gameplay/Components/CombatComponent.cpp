// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Gameplay/Actors/Equippables/Base/BaseWeapon.h"


void UCombatComponent::SetMainWeapon(ABaseWeapon* inMainWeapon)
{
	if (mainWeapon)
	{
		mainWeapon->OnUnequipped();
		mainWeapon->Destroy();
	}
	if (inMainWeapon)
	{
		mainWeapon = inMainWeapon;
	}
}

ABaseWeapon* UCombatComponent::GetMainWeapon() const
{
	return mainWeapon;
}

void UCombatComponent::SetCombatEnabled(bool bIsEnabled)
{
	bIsCombatEnabled = bIsEnabled;
	onToggleCombat.Broadcast(bIsEnabled);
}

void UCombatComponent::ResetCombat()
{
	attackCount = 0;
	bIsCombatEnabled = false;
	bCanContinueAttack = false;
}

