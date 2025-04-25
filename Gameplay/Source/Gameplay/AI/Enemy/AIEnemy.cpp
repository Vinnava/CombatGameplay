// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"

#include "Gameplay/Actors/PatrolSpline.h"
#include "Gameplay/Components/StatsComponent.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values
AAIEnemy::AAIEnemy()
{
	AIPreceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPreceptionComponent");
	StatsComp = CreateDefaultSubobject<UStatsComponent>("StatsComponent");
	patrolSpline = CreateDefaultSubobject<APatrolSpline>("patrolSpline");

}

void AAIEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//PawnSensingComp->OnSeePawn.AddDynamic(this,&AAIEnemy::OnPawnSeen);
	StatsComp->OnHealthChanged.AddDynamic(this, &AAIEnemy::OnHealthChanged);
}

void AAIEnemy::OnHealthChanged_Implementation(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.0f && Delta <= 0.0f)
	{
		AAIEnemy* AIC = Cast<AAIEnemy>(GetController());
		if (AIC)
		{
			UnPossessed();
			Destroy();
		}
	}
}


void AAIEnemy::OnPawnSeen(APawn* Pawn)
{
	if (AAIEnemy* AIC = Cast<AAIEnemy>(GetController()))
	{
		/*
		UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();
		BBComp->SetValueAsObject("TargetActor",Pawn);
		*/

		DrawDebugString(GetWorld(),GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
	}
}


