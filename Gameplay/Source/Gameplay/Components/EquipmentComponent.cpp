 // Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "Gameplay/Actors/Equippables/BaseEquippable.h"
#include "Gameplay/Character/CharacterBase.h"


 void UEquipmentComponent::InitializeEquipment()
 {
	 for (TSubclassOf<ABaseEquippable> startingEquipemet : startingEquipments)
	 {
	 	if (!startingEquipemet) return;
	 	
	 	TObjectPtr<ACharacterBase> character = Cast<ACharacterBase>(GetOwner());
	 	if (!character) return;

	 	// Spawn Parameters
	 	FTransform spawnTransform = character->GetActorTransform();
	 	FActorSpawnParameters SpawnParams;
	 	SpawnParams.Owner = GetOwner();
	 	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	 	
	 	// Spawn the actor
	 	ABaseEquippable* spawnedActor = GetWorld()->SpawnActor<ABaseEquippable>(startingEquipemet, spawnTransform, SpawnParams);

	 	//spawnedActor->OnEquiped();
	 	
	 	currentEquipments.AddUnique(spawnedActor);

	 	//spawnedActor->ToggleCombat();
	 }
 }

 TArray<TObjectPtr<ABaseEquippable>> UEquipmentComponent::GetCurrentEquipments() const
 {
 	return currentEquipments;
 }
