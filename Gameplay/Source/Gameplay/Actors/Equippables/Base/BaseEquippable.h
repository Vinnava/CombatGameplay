// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interface/GameplayTagInterface.h"
#include "BaseEquippable.generated.h"


UCLASS()
class GAMEPLAY_API ABaseEquippable : public AActor, public IGameplayTagInterface
{
	GENERATED_BODY()
	
private://variables
	bool bIsEquipped{false};

protected://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<UPrimitiveComponent> itemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialization|GameplayTag")
	FGameplayTag ownedGameplayTag;

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	FName attachSocketName;

public://Variables

	
private://Functions
	bool AttachActorToSocket(FName socketName);

public:	//Functions
	// Sets default values for this actor's properties
	ABaseEquippable();

	UPrimitiveComponent* GetItemMesh() const { return itemMesh; }

	virtual void OnEquipped();

	virtual void OnUnequipped();

	bool IsEquipped() const { return bIsEquipped; }

	const FGameplayTag& GetOwnedGameplayTag() const override;

	bool HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck) override;
};
