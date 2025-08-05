// Fill out your copyright notice in the Description page of Project Settings.


#include "AICEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Character/Base/CharacterBase.h"
#include "Gameplay/Character/Enemy/EnemyBase.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Data/GameplayTagLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogAICEnemy, Log, All);

AAICEnemy::AAICEnemy()
{
	// Perception Component
	perceptionAIComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp");

	// Sight sense
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	sightConfig->SightRadius = 2000.f;
	sightConfig->LoseSightRadius = 2500.f;
	sightConfig->PeripheralVisionAngleDegrees = 90.f;
	sightConfig->SetMaxAge(5.f);

	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	perceptionAIComp->ConfigureSense(*sightConfig);
	perceptionAIComp->SetDominantSense(sightConfig->GetSenseImplementation());

	// Damage sense
	damageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	perceptionAIComp->ConfigureSense(*damageConfig);
}

void AAICEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (perceptionAIComp)
	{
		perceptionAIComp->OnPerceptionUpdated.AddDynamic(this, &AAICEnemy::OnPerceptionUpdated);
	}
	else UE_LOG(GPLogAICEnemy, Warning, TEXT("[%s] [PostInitializeComponents] perceptionAIComp is null"), *GetName());
}

void AAICEnemy::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	enemyAIRef = Cast<AEnemyBase>(InPawn);

	if (!enemyAIRef)
	{
		UE_LOG(GPLogAICEnemy, Warning, TEXT("[%s] [OnPossess] enemyAIRef is null"), *GetName());
		return;
	}

	//Check IsDummy
	if (enemyAIRef->bIsDummy) return;

	//Run AI
	enemyAIRef->combatComp->onToggleCombat.AddUObject(this, &AAICEnemy::SetBlackboardCombatEnable);
	RunBehaviorTree(enemyAIRef->BehaviorTreeRef);
}

void AAICEnemy::OnPerceptionUpdated(const TArray<AActor*>& sensedActors)
{
	if (enemyAIRef->bIsDummy) return;

	for (AActor* sensedActor : sensedActors)
	{
		ACharacterBase* characterRef = Cast<ACharacterBase>(sensedActor);
		if (!characterRef)
		{
			UE_LOG(GPLogAICEnemy, Error, TEXT("[%s] [OnPerceptionUpdated] CharacterBase cast failed "), *GetName());
			return;
		}
			
		FActorPerceptionBlueprintInfo info;
		perceptionAIComp->GetActorsPerception(sensedActor, info);
		
		for (FAIStimulus lastSensedStimul : info.LastSensedStimuli)
		{
			const FAISenseID senseID = lastSensedStimul.Type;
			const FAISenseID sightID = UAISense::GetSenseID<UAISense_Sight>();
			const FAISenseID damageID = UAISense::GetSenseID<UAISense_Damage>();

			FGameplayTagContainer tagsToCheck;
			tagsToCheck.AddTag(GameplayTags::Player());
			
			if (senseID == sightID) // Handle Sight
			{
				UE_LOG(GPLogAICEnemy, Log, TEXT("[%s] [OnPerceptionUpdated] Sight detected: %s"), *GetName(), *sensedActor->GetName());
				
				if (lastSensedStimul.WasSuccessfullySensed() || characterRef->HasMatchingGameplayTag(tagsToCheck))
				{
					SetBlackboardTagetActor(sensedActor);
				}
				else SetBlackboardTagetActor(nullptr);
			}
			else if (senseID == damageID) // Handle Damage
			{
				UE_LOG(GPLogAICEnemy, Log, TEXT("[%s] [OnPerceptionUpdated] Damage sensed from: %s"), *GetName(), *sensedActor->GetName());

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