// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API AAICEnemy : public AAIController
{
	GENERATED_BODY()

private://Variables
	TObjectPtr<UAIPerceptionComponent> perceptionAIComp;
	
	TObjectPtr<class AEnemyBase> enemyAIRef;
	
protected://Variables


public://Variables


private://Functions
	virtual void OnPossess(APawn* InPawn) override;
	
	void SetBlackboardCombatEnable(bool enableCombat);

protected://Functions

	
public://Functions
	
	
};
