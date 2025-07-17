 // Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "Gameplay/Actors/Equippables/Base/BaseEquippable.h"
#include "Gameplay/Character/Base//CharacterBase.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogEquipementComponent, Log, All)

void UEquipmentComponent::InitializeEquipment()
{
	TObjectPtr<ACharacterBase> character = Cast<ACharacterBase>(GetOwner());
	if (!character)
	{
		UE_LOG(GPLogEquipementComponent, Warning, TEXT("[%s] [InitializeEquipment] Failed to cast owner to ACharacterBase"), *GetName());
		return;
	}

	if (startingEquipments.IsEmpty())
	{
		UE_LOG(GPLogEquipementComponent, Warning, TEXT("[%s] [InitializeEquipment] No starting equipment to spawn"), *GetName());
		return;
	}

	// Set up spawn parameters 
	const FTransform spawnTransform = character->GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (const TSubclassOf<ABaseEquippable>& startingEquipment : startingEquipments)
	{
		if (!startingEquipment)
		{
			UE_LOG(GPLogEquipementComponent, Warning, TEXT("[%s] [InitializeEquipment] Null equipment class found in startingEquipments, skipping"), *GetName());
			continue;
		}
		
		// Spawn the equipment
		ABaseEquippable* spawnedActor = GetWorld()->SpawnActor<ABaseEquippable>(startingEquipment, spawnTransform, SpawnParams);

		// Validate spawn was successful
		if (!spawnedActor)
		{
			UE_LOG(GPLogEquipementComponent, Error, TEXT("[%s] [InitializeEquipment] Failed to spawn equipment: %s"), *GetName(), *startingEquipment->GetName());
			continue;
		}

		// Add to equipment list
		currentEquipments.AddUnique(spawnedActor);
		
		spawnedActor->OnEquipped();
    
		UE_LOG(GPLogEquipementComponent, Log, TEXT("[%s] Successfully spawned equipment: %s"), *GetName(), *spawnedActor->GetName());
	}
}

