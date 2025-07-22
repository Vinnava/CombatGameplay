// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Character/Base/CharacterBase.h"
#include "EnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

private://Variables

	
protected://Variables
	TObjectPtr<class UWidgetComponent> healthWidget;

public://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize|Dummy")
	bool bIsDummy;
	
	UPROPERTY(EditDefaultsOnly, Category = "Initialize|AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeRef;

	UPROPERTY(EditDefaultsOnly, Category = "Initialize|AI")
	TArray<TObjectPtr<class ATargetPoint>> patrolPoints;

private://Functions


protected://Functions
	virtual void BeginPlay() override;
	
public://Functions
	AEnemyBase();

	UFUNCTION(BlueprintCallable, Category = AI)
	void SetHealthBarWidgetVisibility(bool visible);
	
	virtual void OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta) override;
	
	virtual FPerformDeath PerformDeath() override;
};
