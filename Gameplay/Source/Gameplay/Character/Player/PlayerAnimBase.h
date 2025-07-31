// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UPlayerAnimBase : public UAnimInstance
{
	GENERATED_BODY()

private://Variables
	TObjectPtr<class APlayerBase> playerCharacter;
	TObjectPtr<class UCharacterMovementComponent> movementComp;

	FVector velocity;
	
protected://Variables
	UPROPERTY(BlueprintReadOnly, Category = MovementData)
	TObjectPtr<class UCharacterTrajectoryComponent> trajectoryComp;

	UPROPERTY(BlueprintReadOnly, Category = MovementData)
	float groundSpeed;
	
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