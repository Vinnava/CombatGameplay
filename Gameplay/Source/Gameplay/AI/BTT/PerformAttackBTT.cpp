// Fill out your copyright notice in the Description page of Project Settings.


#include "PerformAttackBTT.h"

#include "AIController.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogPerformAttackBTT, Log, All);


EBTNodeResult::Type UPerformAttackBTT::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* masterAIController = OwnerComp.GetAIOwner();
	APawn* masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	if (!masterAIPawn)
	{
		UE_LOG(GPLogPerformAttackBTT, Warning, TEXT("[%s] [ExecuteTask] Pawn is null"), *GetName());
		return EBTNodeResult::Failed;
	}
	
	ICombatInterface* enemyCombatInterface = Cast<ICombatInterface>(masterAIPawn);
	if (!enemyCombatInterface)
	{
		UE_LOG(GPLogPerformAttackBTT, Warning, TEXT("[%s] [ExecuteTask] CombatInterface cast Failed"), *GetName());
		return EBTNodeResult::Failed;
	}

	//Perform Attack
	FPerformAttack performAttackResult = enemyCombatInterface->PerformAttack(attackType, attackIndex, bRandomIndex, true, playRate);
	if (!performAttackResult.bSuccess)
	{
		//Attack Failed
		UE_LOG(GPLogPerformAttackBTT, Warning, TEXT("[%s] [ExecuteTask] Perform Attack Failed"), *GetName());
		return EBTNodeResult::Failed;
	}

	//Modifying Delay Time 
	float modifiedAttackDuration = attackDurationModifier + performAttackResult.attackDuration;
	float modifiedContinueAttackDuration = continueAttackDuration / playRate;

	//Attack Delay
	float delayTime = bContinueAttack? modifiedContinueAttackDuration : modifiedAttackDuration;
	
	FTimerHandle timerHandle;
	FTimerDelegate timerDel;
	timerDel.BindLambda([this, &OwnerComp]()
	{
		//Attack Completed
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	});
	//Attack InProgress
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDel, delayTime, false);
	return EBTNodeResult::InProgress;
}
