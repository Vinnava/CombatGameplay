// Fill out your copyright notice in the Description page of Project Settings.


#include "../Components/StatsComponent.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	HealthMax = 100;
	Health = HealthMax;
}


bool UStatsComponent::IsAlive() const
{
	return Health > 0.0f;
}


float UStatsComponent::GetHealthMax() const
{
	return HealthMax;
}


bool UStatsComponent::ApplyHealthChange(float Delta)
{
	float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);
	
	//UEngine::AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(FString("Health: %f"), Health));

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);
	
	return ActualDelta != 0;
	
}

