// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateEnemyBTS.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Data/GameplayData.h"


void UUpdateEnemyBTS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Get blackboard
	blackboardComp = OwnerComp.GetBlackboardComponent();
}

void UUpdateEnemyBTS::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	
}

void UUpdateEnemyBTS::SetEnemyBehavior(EAIBehaviour newBehaviour)
{
	blackboardComp->SetValueAsEnum(behaviourKey.SelectedKeyName, static_cast<uint8>(newBehaviour));
}

void UUpdateEnemyBTS::UpdateBehavior()
{
	
}
