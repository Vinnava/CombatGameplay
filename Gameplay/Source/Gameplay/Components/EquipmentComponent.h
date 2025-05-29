// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class ABaseEquippable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
private://Variables
	UPROPERTY(EditDefaultsOnly, Category = "Initialization", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ABaseEquippable>> currentEquipments;
	
	TArray<TSubclassOf<ABaseEquippable>> startingEquipments;

public://Functions
	void InitializeEquipment();

	TArray<TObjectPtr<ABaseEquippable>> GetCurrentEquipments() const;
};
