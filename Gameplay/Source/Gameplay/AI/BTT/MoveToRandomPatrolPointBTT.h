// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BTTaskNodeBaseGP.h"
#include "MoveToRandomPatrolPointBTT.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UMoveToRandomPatrolPointBTT : public UBTTaskNodeBaseGP
{
	GENERATED_BODY()
	
protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	float radius{0.0f};
	
	UPROPERTY(EditInstanceOnly, Category=Task)
	FVector queryExtern{300.0f, 300.0f, 300.0f};
	
	UPROPERTY(EditInstanceOnly, Category=Task)
	FBlackboardKeySelector targetLocation;

public://Functions
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UMoveToRandomPatrolPointBTT()
	{
		NodeName = TEXT("MoveToRandomPatrolPoint");
	}
};
