// Fill out your copyright notice in the Description page of Project Settings.


#include "AICEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


void AAICEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr!! Please assign BehaviorTree to your AIController")))
	{
		RunBehaviorTree(BehaviorTree);
	}
	
	/*
	APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (MyPawn)
	{
		//GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsObject("TargetActor",MyPawn);
	}
	*/
}

void AAICEnemy::SetStateAsAttacking_Implementation(AActor* attackTarget)
{
	
}

void AAICEnemy::SetStateAsPassive_Implementation()
{
	
}

