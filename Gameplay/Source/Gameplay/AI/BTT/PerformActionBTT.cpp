// Fill out your copyright notice in the Description page of Project Settings.


#include "PerformActionBTT.h"

#include "AIController.h"
#include "Gameplay/Interface/CombatInterface.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogPerformActionBTT, Log, All);


EBTNodeResult::Type UPerformActionBTT::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* masterAIController = OwnerComp.GetAIOwner();
	APawn* masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	if (!masterAIPawn)
	{
		UE_LOG(GPLogPerformActionBTT, Warning, TEXT("[%s] [ExecuteTask] Pawn is null"), *GetName());
		return EBTNodeResult::Failed;
	}
	
	ICombatInterface* enemyCombatInterface = Cast<ICombatInterface>(masterAIPawn);
	if (!enemyCombatInterface)
	{
		UE_LOG(GPLogPerformActionBTT, Warning, TEXT("[%s] [ExecuteTask] CombatInterface cast Failed"), *GetName());
		return EBTNodeResult::Failed;
	}

	//PerformAction
	FPerformAction PerformActionResult = enemyCombatInterface->PerformAction(characterState, characterAction, montageIndex, bRandomIndex);

	if (!PerformActionResult.bSuccess)
	{
		//Action Failed
		UE_LOG(GPLogPerformActionBTT, Warning, TEXT("[%s] [ExecuteTask] Perform Action Failed"), *GetName());
		return EBTNodeResult::Failed;
	}

	//Modifying Delay Time 
	float delayTime = actionDurationModifier + PerformActionResult.actionDuration;
	
	FTimerHandle timerHandle;
	FTimerDelegate timerDel;
	timerDel.BindLambda([this, &OwnerComp]()
	{
		//Action Completed
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	});
	
	//Action InProgress
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDel, delayTime, false);
	return EBTNodeResult::InProgress;
}
