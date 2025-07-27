// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BTTaskNodeBaseGP.h"
#include "PerformActionBTT.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UPerformActionBTT : public UBTTaskNodeBaseGP
{
	GENERATED_BODY()
	
protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	FGameplayTag characterState;

	UPROPERTY(EditInstanceOnly, Category=Task)
	FGameplayTag characterAction;
	
	UPROPERTY(EditInstanceOnly, Category=Task)
	int32 montageIndex{0};

	UPROPERTY(EditInstanceOnly, Category=Task)
	bool bRandomIndex{false};

	UPROPERTY(EditInstanceOnly, Category=Task)
	float actionDurationModifier{0.0f};
	
public://Functions
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPerformActionBTT()
	{
		NodeName = TEXT("Perform Action");
	}
};
