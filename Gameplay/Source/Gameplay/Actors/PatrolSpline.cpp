// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolSpline.h"

#include "Components/SplineComponent.h"


// Sets default values
APatrolSpline::APatrolSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PatrolSplineComp = CreateDefaultSubobject<USplineComponent>("SplineCompnent");
}

void APatrolSpline::IncrementPatrolRoute()
{
	patrolIndex += patrolDirection;
	if (patrolIndex == PatrolSplineComp->GetNumberOfSplinePoints()-1)
	{
		patrolDirection = -1;
	}
	else if (patrolIndex == 0)
	{
		patrolDirection = 1;
	}
}

FVector APatrolSpline::GetSplinePointsAsWorldPosition()
{
	return PatrolSplineComp->GetLocationAtSplinePoint(patrolIndex,ESplineCoordinateSpace::World);
}



