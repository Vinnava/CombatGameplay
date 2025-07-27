// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_GameplayTaskBase.h"
#include "SetMovementSpeedBTT.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API USetMovementSpeedBTT : public UBTTask_GameplayTaskBase
{
	GENERATED_BODY()

protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	float movementSpeed{230.0f};

public://Functions
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	USetMovementSpeedBTT()
	{
		NodeName = TEXT("Set Movement Speed");
	}
};
