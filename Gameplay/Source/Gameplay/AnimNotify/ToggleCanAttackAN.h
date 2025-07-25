// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ToggleCanAttackAN.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UToggleCanAttackAN : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

public:
	UPROPERTY(EditAnywhere, Category = "Notify")
	bool bCanAttack;
};
