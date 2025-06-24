 // Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "Gameplay/Actors/Equippables/Base/BaseEquippable.h"
#include "Gameplay/Character/Base//CharacterBase.h"

DEFINE_LOG_CATEGORY(LogEquipementComponent)

void UEquipmentComponent::InitializeEquipment()
{
	TObjectPtr<ACharacterBase> character = Cast<ACharacterBase>(GetOwner());
	if (!character)
	{
		UE_LOG(LogEquipementComponent, Warning, TEXT("Failed to cast owner to ACharacterBase"));
		return;
	}

	if (startingEquipments.IsEmpty())
	{
		UE_LOG(LogEquipementComponent, Log, TEXT("No starting equipment to spawn"));
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
			UE_LOG(LogEquipementComponent, Warning, TEXT("Null equipment class found in startingEquipments, skipping"));
			continue;
		}
		
		// Spawn the equipment
		ABaseEquippable* spawnedActor = GetWorld()->SpawnActor<ABaseEquippable>(startingEquipment, spawnTransform, SpawnParams);

		// Validate spawn was successful
		if (!spawnedActor)
		{
			UE_LOG(LogEquipementComponent, Error, TEXT("Failed to spawn equipment: %s"), *startingEquipment->GetName());
			continue;
		}

		// Add to equipment list
		currentEquipments.AddUnique(spawnedActor);
		
		// spawnedActor->OnEquipped();
		// spawnedActor->ToggleCombat();
    
		UE_LOG(LogEquipementComponent, Log, TEXT("Successfully spawned equipment: %s"), *spawnedActor->GetName());
	}
}

