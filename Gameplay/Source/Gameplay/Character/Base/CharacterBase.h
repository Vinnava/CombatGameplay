// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interface/CombatInterface.h"
#include "Gameplay/Interface/GameplayTagInterface.h"
#include "Logging/LogMacros.h"
#include "CharacterBase.generated.h"


UCLASS(config=Game)
class ACharacterBase : public ACharacter, public IGameplayTagInterface, public ICombatInterface
{
	GENERATED_BODY()

private://Variables
	enum class EMovementSpeedMode movementSpeedMode;
	enum class EHitDirection hitDirection;
	
protected://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UMotionWarpingComponent> motionWarpingComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialize)
	FGameplayTag ownedGameplayTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialize)
	FGameplayTag ignoreGameplayTag;

	UPROPERTY(EditDefaultsOnly, Category = Initialize)
	FName pelvisBoneName;

	UPROPERTY(EditDefaultsOnly, Category = "Initialize|MotionWarping")
	FName attackWarpTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Initialize|MovementSpeed", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float maxWalkSpeed {200.0f};
	UPROPERTY(EditDefaultsOnly, Category = "Initialize|MovementSpeed", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float maxJogSpeed {500.0f};
	UPROPERTY(EditDefaultsOnly, Category = "Initialize|MovementSpeed", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float maxSprintSpeed {750.0f};

public://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStatsComponent> statsComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UEquipmentComponent> equipComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStateManagerComponent> stateManagerComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCombatComponent> combatComp;
	
private://Functions
	void ApplyDamage(bool bCanDamage, float damage, AController* instigatorController, FVector hitLocation);

#pragma region ComponentDelegates

	UFUNCTION()
	virtual void OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta);
	virtual void OnCharacterStateBegin(FGameplayTag characterState);
#pragma endregion ComponentDelegates

protected://Functions
	virtual void BeginPlay() override;

	virtual float TakeDamage(float damage, FDamageEvent const& damageEvent, 
							AController* eventInstigator, AActor* damageCauser) override;

	void Attack();
	virtual void EnableRagdoll() const;

	bool CanPerformAttack() const;
	bool CanPerformDodge() const;
	void SetMovementSpeedMode(EMovementSpeedMode newMovementSpeedMode);
	EMovementSpeedMode GetMovementSpeedMode() const;
	EHitDirection UpdateAndGetHitDirection(FVector hitLocation);
	FRotator GetDesiredRotation() const;
	
public://Functions
	ACharacterBase();
	virtual void PostInitializeComponents() override;
	
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

