// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()

private://Variables

	
protected://Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UCollisionComponent> collisionComp;

public://Variables
	ABaseWeapon();

private://Functions


protected://Functions

	
public://Functions

};
