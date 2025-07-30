// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UEnemyAnimBase : public UAnimInstance
{
	GENERATED_BODY()

private://Variables
	TObjectPtr<class AEnemyBase> enemyAICharacter;
	TObjectPtr<class UMovementComponent> movementComp;

	FVector velocity;
	float GroundSpeed;
	
protected://Variables
	UPROPERTY(BlueprintReadOnly, Category = MovementData)
	bool bShouldMove;
	
	UPROPERTY(BlueprintReadOnly, Category = MovementData)
	bool bIsFalling;

public://Variables


private://Functions


protected://Functions

	
public://Functions
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
