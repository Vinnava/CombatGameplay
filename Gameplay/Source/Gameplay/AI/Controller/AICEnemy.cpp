// Fill out your copyright notice in the Description page of Project Settings.


#include "AICEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Character/Base/CharacterBase.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Data/GameplayTagData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"


AAICEnemy::AAICEnemy()
{
	perceptionAIComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp");
}

void AAICEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	perceptionAIComp->OnPerceptionUpdated.AddDynamic(this, &AAICEnemy::OnPerceptionUpdated);
}

void AAICEnemy::OnPossess(APawn* InPawn)
{
	enemyAIRef = Cast<AEnemyBase>(InPawn);

	if (enemyAIRef->bIsDummy) return;
	enemyAIRef->combatComp->onToggleCombat.AddUObject(this, &AAICEnemy::SetBlackboardCombatEnable);
	RunBehaviorTree(enemyAIRef->BehaviorTreeRef);
}

void AAICEnemy::OnPerceptionUpdated(const TArray<AActor*>& sensedActors)
{
	if (enemyAIRef->bIsDummy) return;

	for (AActor* sensedActor : sensedActors)
	{
		ACharacterBase* characterRef = Cast<ACharacterBase>(sensedActor);
		if (!characterRef) return;
			
		FActorPerceptionBlueprintInfo info;
		perceptionAIComp->GetActorsPerception(sensedActor, info);
		
		for (FAIStimulus lastSensedStimul : info.LastSensedStimuli)
		{
			const FAISenseID senseID = lastSensedStimul.Type;
			const FAISenseID sightID = UAISense::GetSenseID<UAISense_Sight>();
			const FAISenseID damageID = UAISense::GetSenseID<UAISense_Damage>();

			FGameplayTagContainer tagsToCheck;
			tagsToCheck.AddTag(playerTag);
			
			if (senseID == sightID) // Handle Sight
			{
				UE_LOG(LogTemp, Warning, TEXT("Sight detected: %s"), *sensedActor->GetName());
				
				if (lastSensedStimul.WasSuccessfullySensed() || characterRef->HasMatchingGameplayTag(tagsToCheck))
				{
					SetBlackboardTagetActor(sensedActor);
				}
				else SetBlackboardTagetActor(nullptr);
			}
			else if (senseID == damageID) // Handle Damage
			{
				UE_LOG(LogTemp, Warning, TEXT("Damage sensed from: %s"), *sensedActor->GetName());

				if (lastSensedStimul.WasSuccessfullySensed() || characterRef->HasMatchingGameplayTag(tagsToCheck))
				{
					SetBlackboardTagetActor(sensedActor);
				}
			}
		}
	}
}

void AAICEnemy::SetBlackboardCombatEnable(bool enableCombat)
{
	GetBlackboardComponent()->SetValueAsBool("bCombatEnable", enableCombat);
}

void AAICEnemy::SetBlackboardTagetActor(AActor* actor)
{
	GetBlackboardComponent()->SetValueAsObject("Target",actor);
}

void AAICEnemy::ToggleCanAttack(bool bCanAttack)
{
	if (enemyAIRef->bIsDummy) return;

	GetBlackboardComponent()->SetValueAsBool("bCanAttack", bCanAttack);
}