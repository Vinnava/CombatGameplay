// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()

private://Variables

	
protected://Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<class UCollisionComponent> collisionComp;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialization)
	//FName attachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TArray<TObjectPtr<UAnimMontage>> lightAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TArray<TObjectPtr<UAnimMontage>> dodgeMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TArray<TObjectPtr<UAnimMontage>> dieMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TObjectPtr<UAnimMontage> hitMontage_F;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TObjectPtr<UAnimMontage> hitMontage_B;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TObjectPtr<UAnimMontage> hitMontage_R;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations)
	TObjectPtr<UAnimMontage> hitMontage_L;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	TObjectPtr<class UCombatComponent> combatComp;

public://Variables
	ABaseWeapon();
	void BeginPlay() override;

	void OnEquipped() override;
	void OnHit(FHitResult hitResult);
	void SimulateWeaponPhysics();
	
	void ActivateCollision();
	void DeactivateCollision();
	
	void ToggleCombat();
	
	float GetDamage() const { return damage; }
	TArray<TObjectPtr<UAnimMontage>> GetActioMontages(FGameplayTag characterAction) const;

private://Functions


protected://Functions

	
public://Functions

};
