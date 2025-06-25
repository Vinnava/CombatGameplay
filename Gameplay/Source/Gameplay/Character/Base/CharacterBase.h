// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interface/CombatInterface.h"
#include "Gameplay/Interface/GameplayTagInterface.h"
#include "Logging/LogMacros.h"
#include "CharacterBase.generated.h"



DECLARE_LOG_CATEGORY_EXTERN(LogBaseCharacter, Log, All);

UCLASS(config=Game)
class ACharacterBase : public ACharacter, public IGameplayTagInterface, public ICombatInterface
{
	GENERATED_BODY()

private://Variables
	UPROPERTY(EditDefaultsOnly, Category = Initialize)
	FName pelvisBoneName;

	EMovementMode movementSpeedMode;
	enum class EHitDirection hitDirection;
	
protected://Variables
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStatsComponent> statsComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Components)
	TObjectPtr<class UStateManagerComponent> stateManagerComp ;

	TObjectPtr<class UCombatComponent> combatComp;
	TObjectPtr<class UEquipmentComponent> equipComp;
	TObjectPtr<class UMotionWarpingComponent> motionWarpingComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialize)
	FGameplayTag ownedGameplayTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialize)
	FGameplayTag ignoreGameplayTag;
	
	float maxWalkSpeed;
	float maxJogSpeed;
	float maxSprintSpeed;

public://Variables

private://Functions
	void DestroyAttachedActorsAndSelf(const TArray<AActor*>& actorsArray);
	void EnableRagdoll();

protected://Functions
	virtual void BeginPlay();

	bool CanPerformAttack() const;
	
#pragma region StatsComponentDelegates

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta);
	void OnCharacterStateBegin(FGameplayTag characterState);
#pragma endregion StatsComponentDelegates
	
public://Functions
	ACharacterBase();
	virtual void PostInitializeComponents() override;

	void Attack();
	
#pragma region IGameplayTagInterface
	
	virtual const FGameplayTag& GetOwnedGameplayTag() const override;
	virtual bool HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck) override;
#pragma endregion IGameplayTagInterface 

#pragma region ICombatInterface

	virtual void ContinueAttack() override;
	virtual void ResetCombat() override;
	virtual bool CanReciveDamage() override;
	virtual FPerformDeath PerformDeath() override;
	virtual bool PerformHitReaction(FVector hitLocation, float damage) override;
	virtual FPerformAction PerformAction(FGameplayTag characterState,
										FGameplayTag characterAction,
										int32 montageIndex,
										bool bRandomIndex) override;
	virtual FPerformAttack PerformAttack(FGameplayTag attackType,
										int32 attackIndex,
										bool bRandomIndex,
										bool bIsCalledByAI,
										float playRate) override;
#pragma endregion ICombatInterface
};

