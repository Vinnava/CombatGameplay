// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/WidgetComponent.h"
#include "Gameplay/UI/Player/HealthBarWidget.h"


AEnemyBase::AEnemyBase()
{
	healthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	healthWidget->SetupAttachment(GetMesh(), TEXT("head"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	SetHealthBarWidgetVisibility(false);
}

void AEnemyBase::SetHealthBarWidgetVisibility(bool visible)
{
	healthWidget->SetVisibility(visible);
}

void AEnemyBase::OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta)
{
	if (UHealthBarWidget* healthBarWidgetRef = Cast<UHealthBarWidget>(healthWidget))
	{
		healthBarWidgetRef->UpdateHealth(newHealth);
	}
}

FPerformDeath AEnemyBase::PerformDeath()
{
	SetHealthBarWidgetVisibility(false);
	return Super::PerformDeath();
}
