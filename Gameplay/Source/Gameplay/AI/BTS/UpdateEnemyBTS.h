// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "UpdateEnemyBTS.generated.h"


/**
 * 
 */
UCLASS()
class GAMEPLAY_API UUpdateEnemyBTS : public UBTService
{
	GENERATED_BODY()
	
private://Variables
	TObjectPtr<UBlackboardComponent> blackboardComp;
	TObjectPtr<APawn> masterAI;
	TObjectPtr<AAIController> masterAIController;
	bool bCanSeeTarget{false};

protected://Variables
	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector behaviourKey;

	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector target;

	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector self;

	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector maxAttackRange;

private://Functions
	void SetEnemyBehavior(enum class EAIBehaviour newBehaviour);
	void UpdateBehavior();
	
protected://Functions
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

public://Functions
	UUpdateEnemyBTS()
	{
		NodeName = TEXT("Update Enemy Behavior");
		bNotifyBecomeRelevant = true;
		bNotifyTick = true;
	}
};
