// Fill out your copyright notice in the Description page of Project Settings.


#include "SetMovementSpeedBTT.h"

#include "AIController.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogSetMovementSpeedBTT, Log, All);


EBTNodeResult::Type USetMovementSpeedBTT::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* masterAIController = OwnerComp.GetAIOwner();
	APawn* masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	if (!masterAIPawn)
	{
		UE_LOG(GPLogSetMovementSpeedBTT, Warning, TEXT("[%s] [ExecuteTask] Pawn is null"), *GetName());
		return EBTNodeResult::Failed;
	}

	AEnemyBase* enemyAIRef = Cast<AEnemyBase>(masterAIPawn);
	if (!enemyAIRef)
	{
		UE_LOG(GPLogSetMovementSpeedBTT, Warning,  TEXT("[%s] [ExecuteTask] Enemy cast Failed"), *GetName());
		return EBTNodeResult::Failed;
	}

	enemyAIRef->SetMovementSpeedMode(movementSpeedMode);
	return EBTNodeResult::Succeeded;
}
