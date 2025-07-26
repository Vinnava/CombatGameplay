// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BTTaskNodeBaseGP.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "PerformAttackBTT.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UPerformAttackBTT : public UBTTaskNodeBaseGP
{
	GENERATED_BODY()
	
protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	FGameplayTag attackType;

	UPROPERTY(EditInstanceOnly, Category=Task)
	int32 attackIndex{0};

	UPROPERTY(EditInstanceOnly, Category=Task)
	bool bRandomIndex{false};

	UPROPERTY(EditInstanceOnly, Category=Task)
	float attackDurationModifier{0.0f};

	UPROPERTY(EditInstanceOnly, Category=Task)
	float playRate{1.0f};

	UPROPERTY(EditInstanceOnly, Category=Task)
	bool bContinueAttack{false};

	UPROPERTY(EditInstanceOnly, Category=Task)
	float continueAttackDuration{0.0f};
	
public://Functions
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPerformAttackBTT()
	{
		NodeName = TEXT("Perform Light Attack");
	}
};
