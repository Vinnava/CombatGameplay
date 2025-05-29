// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEquippable.generated.h"

UCLASS()
class GAMEPLAY_API ABaseEquippable : public AActor
{
	GENERATED_BODY()
	
private://variables
	bool bIsEquipped{false};

protected://Functions
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public://Variables

	
public:	//Functions
	// Sets default values for this actor's properties
	ABaseEquippable();

};
