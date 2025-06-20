// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interface/GameplayTagInterface.h"
#include "BaseEquippable.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogBaseEquippable, Log, All);

UCLASS()
class GAMEPLAY_API ABaseEquippable : public AActor, public IGameplayTagInterface
{
	GENERATED_BODY()
	
private://variables
	bool bIsEquipped{false};

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	FName attachSocketName;

protected://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	TObjectPtr<UPrimitiveComponent> itemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayTag)
	FGameplayTag ownedGameplayTag;

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
