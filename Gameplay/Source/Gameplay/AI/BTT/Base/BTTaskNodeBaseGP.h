// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNodeBaseGP.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UBTTaskNodeBaseGP : public UBTTaskNode
{
	GENERATED_BODY()

protected://Variables
	
	/** show detailed information about properties */
	UPROPERTY(EditInstanceOnly, Category=Description)
	bool bShowPropertyDetails {true};
	
	/** properties with runtime values, stored only in class default object */
	TArray<FProperty*> PropertyData;

protected://Function
	// Gets the description for our service
	virtual FString GetStaticDescription() const override;

public://Function
	UBTTaskNodeBaseGP();
};
