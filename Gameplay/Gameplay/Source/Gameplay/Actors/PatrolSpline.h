// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolSpline.generated.h"

class USplineComponent;

UCLASS()
class GAMEPLAY_API APatrolSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolSpline();
	
//Functions
	UFUNCTION(BlueprintCallable, Category = "AI|Patrol" )
	void IncrementPatrolRoute();

	UFUNCTION(BlueprintCallable, Category = "AI|Patrol" )
	FVector GetSplinePointsAsWorldPosition();

	
protected:
//Functions
	
	
//Variables
	UPROPERTY(VisibleAnywhere,Category="Components")
	TObjectPtr<USplineComponent> PatrolSplineComp;
	
	UPROPERTY(VisibleAnywhere, Category="AI|Patrol")
	int patrolIndex;

	UPROPERTY(VisibleAnywhere, Category="AI|Patrol", meta = (ClampMin = "-1", ClampMax = "1"))
	int patrolDirection = 1;
	
};
