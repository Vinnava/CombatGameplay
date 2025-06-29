// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayData.generated.h"


enum class EHitDirection
{
	Front,
	Back,
	Left,
	Right
};

enum class EMovementSpeedMode
{
	Walking,
	Jogging,
	Sprinting,
};

UCLASS()
class GAMEPLAY_API UGameplayData : public UObject
{
	GENERATED_BODY()
	
};
