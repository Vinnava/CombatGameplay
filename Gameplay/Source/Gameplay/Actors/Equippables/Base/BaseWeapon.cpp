// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Gameplay/Components/CollisionComponent.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Data/GameplayTagData.h"
#include "Kismet/GameplayStatics.h"


ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	collisionComp = CreateDefaultSubobject<UCollisionComponent>("CollisionComp");
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	collisionComp->onHitCollision.AddUObject(this, &ABaseWeapon::OnHit);
}

void ABaseWeapon::OnEquipped()
{
	Super::OnEquipped();

	combatComp = GetOwner()->GetComponentByClass<UCombatComponent>();
	combatComp->SetMainWeapon(this);

	collisionComp->SetCollisionMeshComponent(itemMesh);
	collisionComp->AddIgnoreActor(GetOwner());
}

void ABaseWeapon::OnHit(FHitResult hitResult)
{
	UGameplayStatics::ApplyPointDamage(hitResult.GetActor(),
							GetDamage(),
							GetOwner()->GetActorForwardVector(),
							hitResult,
							GetInstigatorController(),
							this,
							UDamageType::StaticClass());
}

void ABaseWeapon::SimulateWeaponPhysics()
{
	GetItemMesh()->SetCollisionProfileName("PhysicalActor", true);
	GetItemMesh()->SetSimulatePhysics(true);
}

void ABaseWeapon::ActivateCollision()
{
	collisionComp->ActivateCollision();
}

void ABaseWeapon::DeactivateCollision()
{
	collisionComp->DeactivateCollision();
}

void ABaseWeapon::ToggleCombat()
{
	combatComp->SetCombatEnabled(true);
}

const TArray<TObjectPtr<UAnimMontage>>& ABaseWeapon::GetActioMontages(FGameplayTag characterAction) const
{
	if (characterAction.MatchesTagExact(lightAttackActionTag))
	{
		return lightAttackMontages;
	}
	else if (characterAction.MatchesTagExact(dodgeActionTag))
	{
		return dodgeMontages;
	}
	else if (characterAction.MatchesTagExact(dieActionTag))
	{
		return dieMontages;
	}
	else
	{
		static const TArray<TObjectPtr<UAnimMontage>> EmptyMontages;
		return EmptyMontages;
	}
}

