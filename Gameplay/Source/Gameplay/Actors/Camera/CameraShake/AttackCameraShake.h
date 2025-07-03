// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shakes/LegacyCameraShake.h"
#include "AttackCameraShake.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UAttackCameraShake : public ULegacyCameraShake
{
	GENERATED_BODY()

public:
	UAttackCameraShake();
};
