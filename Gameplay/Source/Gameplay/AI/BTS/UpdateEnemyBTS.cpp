// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateEnemyBTS.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"
#include "Gameplay/Components/StateManagerComponent.h"
#include "Gameplay/Data/GameplayData.h"
#include "Gameplay/Data/GameplayTagLibrary.h"


DEFINE_LOG_CATEGORY_STATIC(GPLogUpdateEnemyBTS, Log, All);

void UUpdateEnemyBTS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Get blackboard
	blackboardComp = OwnerComp.GetBlackboardComponent();
	masterAIController = OwnerComp.GetAIOwner();
	masterAIPawn = masterAIController ? masterAIController->GetPawn() : nullptr;
	UpdateBehavior();
}

void UUpdateEnemyBTS::SetEnemyBehavior(EAIBehaviour newBehaviour)
{
	//Updating behaviourKey Value
	blackboardComp->SetValueAsEnum(behaviourKey.SelectedKeyName, static_cast<uint8>(newBehaviour));
}

void UUpdateEnemyBTS::UpdateBehavior()
{
	if (!masterAIPawn && !masterAIController)
	{
		UE_LOG(GPLogUpdateEnemyBTS, Error, TEXT("[%s] [UpdateBehavior] Couldn't able to update behaviour : masterAI or masterAIController is null"), *GetName());
		return;
	}

	AEnemyBase* enemyAIRef = Cast<AEnemyBase>(masterAIPawn);
	if (!enemyAIRef)
	{
		UE_LOG(GPLogUpdateEnemyBTS, Warning, TEXT("[%s] [UpdateBehavior] AEnemyBase cast Failed"), *GetName());
		return;
	}

	if (enemyAIRef->stateManagerComp->GetCurrentState() == GameplayTags::State::Dead())
	{
		//Dead
		SetEnemyBehavior(EAIBehaviour::None);
	}
	else
	{
		AActor* targetActor = UBTFunctionLibrary::GetBlackboardValueAsActor(this, target);
		if (!targetActor)
		{
			//Player Wasn't Deducted
			bCanSeeTarget = false;
			SetEnemyBehavior(EAIBehaviour::Patrol);
			enemyAIRef->SetHealthBarWidgetVisibility(false);
		}
		else
		{
			enemyAIRef->SetHealthBarWidgetVisibility(true);
			bCanSeeTarget = true;

			float targetDistance = targetActor->GetDistanceTo(masterAIPawn);
			if (targetDistance <= maxAttackRange)
			{
				//In AttackRange
				SetEnemyBehavior(EAIBehaviour::Attack);
			}
			//Out OF Range, Chase & Attack
			else SetEnemyBehavior(EAIBehaviour::Chase);
		}
	}
}
