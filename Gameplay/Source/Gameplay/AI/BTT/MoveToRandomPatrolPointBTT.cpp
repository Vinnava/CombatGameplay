// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToRandomPatrolPointBTT.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogRandomPatrolPointBTT, Log, All);


EBTNodeResult::Type UMoveToRandomPatrolPointBTT::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                             uint8* NodeMemory)
{
	AAIController* masterAIController = OwnerComp.GetAIOwner();
	APawn* masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	if (!masterAIPawn)
	{
		UE_LOG(GPLogRandomPatrolPointBTT, Warning, TEXT("[%s] [ExecuteTask] Pawn is null"), *GetName());
		return EBTNodeResult::Failed;
	}
	
	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!navSystem)
	{
		UE_LOG(GPLogRandomPatrolPointBTT, Warning, TEXT("[%s] [ExecuteTask] Failed to find a reachable point in radius %.2f"), *GetName(), radius);
		return EBTNodeResult::Failed;
	}

	//Get Enemy Location
	FVector enemyAILocation = masterAIPawn->GetActorLocation();
	FNavLocation outLocation;
	
	//Random Reachable Point In Radius
	bool bFound = navSystem->GetRandomReachablePointInRadius(enemyAILocation, radius, outLocation);
	if (!bFound)
	{
		UE_LOG(GPLogRandomPatrolPointBTT, Warning, TEXT("[%s] [ExecuteTask] Failed to find a reachable point"), *GetName());
		return EBTNodeResult::Failed;
	}

	//Random Location
	FVector randomLocation = outLocation.Location;

	//Point To Navigation
	bFound = navSystem->ProjectPointToNavigation(randomLocation, outLocation);
	if (!bFound)
	{
		UE_LOG(GPLogRandomPatrolPointBTT, Warning, TEXT("[%s] [ExecuteTask] Failed point to navigation"), *GetName());
		return EBTNodeResult::Failed;
	}

	//Random Point of Navigation
	randomLocation = outLocation.Location;

	// Get blackboard
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)
	{
		UE_LOG(GPLogRandomPatrolPointBTT, Warning, TEXT("[%s] [ExecuteTask] Failed to get blackboardComp"), *GetName());
		return EBTNodeResult::Failed;
	}

	blackboardComp->SetValueAsVector(targetLocation.SelectedKeyName, randomLocation);
	return EBTNodeResult::Succeeded;
}
