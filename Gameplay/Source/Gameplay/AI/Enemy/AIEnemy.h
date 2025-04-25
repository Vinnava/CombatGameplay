// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIEnemy.generated.h"

class UAIPerceptionComponent;
class APatrolSpline;
class UStatsComponent;


UCLASS()
class GAMEPLAY_API AAIEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIEnemy();

	
//Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Patrol")
	TObjectPtr<APatrolSpline> patrolSpline;

protected:
	
	virtual void PostInitializeComponents() override;
	
//Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIPerceptionComponent> AIPreceptionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Components")
	TObjectPtr<UStatsComponent> StatsComp;
	
	
//Functions
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnHealthChanged(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta);
	
};
