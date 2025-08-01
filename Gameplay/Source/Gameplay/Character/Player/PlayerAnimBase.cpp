// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay//Character/Player/PlayerAnimBase.h"

#include "CharacterTrajectoryComponent.h"
#include "PlayerBase.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogPlayerAnimBase, Log, All);

void UPlayerAnimBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//Character
	playerCharacter = Cast<APlayerBase>(GetOwningActor());
	if (!playerCharacter)
	{
		UE_LOG(GPLogPlayerAnimBase, Error, TEXT("[%s] [NativeInitializeAnimation] playerCharacter returns null"), *GetName());
		return;
	}

	//Movement Component
	movementComp = playerCharacter->GetCharacterMovement();
	if (!movementComp) UE_LOG(GPLogPlayerAnimBase, Error, TEXT("[%s] [NativeInitializeAnimation] movementComp returns null"), *GetName());

	//Trajectory Component
	trajectoryComp = playerCharacter->FindComponentByClass<UCharacterTrajectoryComponent>();
	if (!trajectoryComp) UE_LOG(GPLogPlayerAnimBase, Error, TEXT("[%s] [NativeInitializeAnimation] trajectoryComp returns null"), *GetName());
}

void UPlayerAnimBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!playerCharacter || !movementComp)
	{
		UE_LOG(GPLogPlayerAnimBase, Error, TEXT("[%s] [NativeUpdateAnimation] playerCharacter or movementComp returns null"), *GetName());
		return;
	}

	//Velocity
	velocity = movementComp->GetLastUpdateVelocity();

	//GroundSpeed
	groundSpeed = UKismetMathLibrary::VSizeXY(velocity);

	//Should Move
	bShouldMove = groundSpeed > 3.0f && !movementComp->GetCurrentAcceleration().IsNearlyZero();

	//Is Falling
	bIsFalling = movementComp->IsFalling();
}
