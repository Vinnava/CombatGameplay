// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged,
											AActor*, instigatorActor,
											UStatsComponent*, owningComp,
											float, newHealth,
											float, delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

protected:
//Variables
	UPROPERTY(VisibleAnywhere, Category="Attributes")
	float health;

	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	float healthMax;

public:
//Function
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsAlive() const;
	
	UFUNCTION(BlueprintCallable)
	float GetHealthMax() const;

//Variables
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHealthChanged OnHealthChanged;	
};
