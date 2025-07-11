// Fill out your copyright notice in the Description page of Project Settings.


#include "AICEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"
#include "Gameplay/Components/CombatComponent.h"


void AAICEnemy::OnPossess(APawn* InPawn)
{
	enemyAIRef = Cast<AEnemyBase>(InPawn);

	if (enemyAIRef->bIsDummy) return;
	enemyAIRef->combatComp->onToggleCombat.AddUObject(this, &AAICEnemy::SetBlackboardCombatEnable);
	RunBehaviorTree(enemyAIRef->BehaviorTreeRef);
}

void AAICEnemy::SetBlackboardCombatEnable(bool enableCombat)
{
	GetBlackboardComponent()->SetValueAsBool("bCombatEnable", enableCombat);
}
