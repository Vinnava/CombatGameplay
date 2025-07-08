// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

private://Variables

	
protected://Variables


public://Variables


private://Functions


protected://Functions

	
public://Functions
	UFUNCTION(BlueprintImplementableEvent ,Category = "HealthBarWidget")
	void UpdateHealth(float health); 
};
