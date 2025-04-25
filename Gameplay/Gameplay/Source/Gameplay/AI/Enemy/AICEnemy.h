// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API AAICEnemy : public AAIController
{
	GENERATED_BODY()
		
//Functions
	virtual void BeginPlay() override;


//Variables	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

public:
//Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|State")
	void SetStateAsPassive();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|State")
	void SetStateAsAttacking(AActor* attackTarget);
	
};
