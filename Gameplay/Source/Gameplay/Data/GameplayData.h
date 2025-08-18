// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayData.generated.h"


enum class EHitDirection
{
	None,
	Front,
	Back,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EMovementSpeedMode : uint8
{
	None		UMETA(DisplayName = "None"),
	Walking     UMETA(DisplayName = "Walking"),
	Jogging     UMETA(DisplayName = "Jogging"),
	Sprinting   UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EAIBehaviour : uint8
{
	None      UMETA(DisplayName = "None"),
	Patrol    UMETA(DisplayName = "Patrol"),
	Chase     UMETA(DisplayName = "Chase"),
	Attack    UMETA(DisplayName = "Attack"),
	Hit       UMETA(DisplayName = "Hit"),
	Dead      UMETA(DisplayName = "Dead"),
};

UCLASS()
class GAMEPLAY_API UGameplayData : public UObject
{
	GENERATED_BODY()
	
};
