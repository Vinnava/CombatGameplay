// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BTTaskNodeBaseGP.h"
#include "Gameplay/Data/GameplayData.h"
#include "SetMovementSpeedBTT.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API USetMovementSpeedBTT : public UBTTaskNodeBaseGP
{
	GENERATED_BODY()

protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	EMovementSpeedMode movementSpeedMode{EMovementSpeedMode::Idle};

public://Functions
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	USetMovementSpeedBTT()
	{
		NodeName = TEXT("Set Movement Speed");
	}
};
