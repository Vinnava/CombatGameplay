// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BTServiceBaseGP.h"
#include "UpdateEnemyBTS.generated.h"


/**
 * 
 */
UCLASS()
class GAMEPLAY_API UUpdateEnemyBTS : public UBTServiceBaseGP
{
	GENERATED_BODY()
	
private://Variables
	TObjectPtr<UBlackboardComponent> blackboardComp;
	TObjectPtr<APawn> masterAIPawn;
	TObjectPtr<AAIController> masterAIController;
	bool bCanSeeTarget{false};

protected://Variables
	UPROPERTY(EditInstanceOnly, Category=Task)
	FBlackboardKeySelector behaviourKey;

	UPROPERTY(EditInstanceOnly, Category=Task)
	FBlackboardKeySelector target;

	UPROPERTY(EditInstanceOnly, Category=Task)
	FBlackboardKeySelector attackRangeKey;

	UPROPERTY(EditInstanceOnly, Category=Task)
	float maxAttackRange{0.0f};

private://Functions
	void SetEnemyBehavior(enum class EAIBehaviour newBehaviour);
	void UpdateBehavior();
	
protected://Functions
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public://Functions
	UUpdateEnemyBTS()
	{
		NodeName = TEXT("Update EnemyAI Behavior");
		bNotifyBecomeRelevant = true;
		bNotifyTick = true;
	}
};
