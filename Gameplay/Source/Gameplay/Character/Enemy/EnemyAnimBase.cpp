// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimBase.h"

#include "EnemyBase.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogEnemyAnimBase, Log, All);

void UEnemyAnimBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//Character
	enemyAICharacter = Cast<AEnemyBase>(GetOwningActor());
	if (!enemyAICharacter)
	{
		UE_LOG(GPLogEnemyAnimBase, Error, TEXT("[%s] [NativeInitializeAnimation] enemyAICharacter returns null"), *GetName());
		return;
	}

	//Movement Component
	movementComp = enemyAICharacter->GetCharacterMovement();
	if (!movementComp) UE_LOG(GPLogEnemyAnimBase, Error, TEXT("[%s] [NativeInitializeAnimation] movementComp returns null"), *GetName());
}

void UEnemyAnimBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!enemyAICharacter || !movementComp)
	{
		UE_LOG(GPLogEnemyAnimBase, Error, TEXT("[%s] [NativeUpdateAnimation] enemyAICharacter or movementComp returns null"), *GetName());
		return;
	}

	//Velocity
	velocity = movementComp->GetLastUpdateVelocity();

	//GroundSpeed
	groundSpeed = UKismetMathLibrary::VSizeXY(velocity);

	//Should Move
	bShouldMove = groundSpeed > 3.0f ;//&& !movementComp->GetCurrentAcceleration().IsNearlyZero();

	//Is Falling
	bIsFalling = movementComp->IsFalling();
}
