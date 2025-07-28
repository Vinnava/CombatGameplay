// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayData.generated.h"


enum class EHitDirection
{
	Front,
	Back,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EMovementSpeedMode : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Walking     UMETA(DisplayName = "Walking"),
	Jogging     UMETA(DisplayName = "Jogging"),
	Sprinting   UMETA(DisplayName = "Sprinting")
};

enum class EAIBehaviour
{
	None,
	Patrol,
	Chase,
	Attack,
	Hit,
	Dead
};

UCLASS()
class GAMEPLAY_API UGameplayData : public UObject
{
	GENERATED_BODY()
	
};
