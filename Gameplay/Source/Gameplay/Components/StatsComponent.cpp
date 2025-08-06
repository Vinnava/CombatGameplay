// Fill out your copyright notice in the Description page of Project Settings.


#include "../Components/StatsComponent.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	healthMax = 100;
	health = healthMax;
}

bool UStatsComponent::IsAlive() const
{
	return health > 0.0f;
}

float UStatsComponent::GetHealthMax() const
{
	return healthMax;
}

bool UStatsComponent::ApplyHealthChange(float Delta)
{
	const float OldHealth = health;
	
	health = FMath::Clamp(health + Delta, 0.0f, healthMax);

	float ActualDelta = health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, health, ActualDelta);
	
	return ActualDelta != 0;
}

