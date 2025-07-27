// Fill out your copyright notice in the Description page of Project Settings.


#include "SetMovementSpeedBTT.h"

#include "AIController.h"
#include "GameFramework/CharactermovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogPerformActionBTT, Log, All);


EBTNodeResult::Type USetMovementSpeedBTT::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* masterAIController = OwnerComp.GetAIOwner();
	APawn* masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	if (!masterAIPawn)
	{
		UE_LOG(GPLogPerformActionBTT, Warning, TEXT("[%s] [ExecuteTask] Pawn is null"), *GetName());
		return EBTNodeResult::Failed;
	}

	if (UCharacterMovementComponent* movementComp = masterAIPawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		movementComp->MaxWalkSpeed = movementSpeed;
		return EBTNodeResult::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MovementComponent not found on AI Pawn: %s"), *masterAIPawn->GetName());
		return EBTNodeResult::Failed;
	}
}
