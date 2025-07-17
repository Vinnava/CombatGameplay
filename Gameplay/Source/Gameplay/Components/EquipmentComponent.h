// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
	
private://Variables
	TArray<TObjectPtr<class ABaseEquippable>> currentEquipments;

	UPROPERTY(EditDefaultsOnly, Category = "Initialization", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ABaseEquippable>> startingEquipments;

public://Functions
	void InitializeEquipment();

	TArray<TObjectPtr<ABaseEquippable>> GetCurrentEquipments() const { return currentEquipments; }
};
