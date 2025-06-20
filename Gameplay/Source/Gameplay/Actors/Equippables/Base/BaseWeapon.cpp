// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Gameplay/Components/CollisionComponent.h"
#include "Gameplay/Components/CombatComponent.h"
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

TArray<TObjectPtr<UAnimMontage>> ABaseWeapon::GetActioMontages(FGameplayTag characterAction) const
{
	switch (characterAction)
	{
		case characterAction.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.LightAttack"))):
			return lightAttackMontages;
		case characterAction.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Character.Action.Dodge"))):
			return dodgeMontages;
		case characterAction.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Character.Action.Die"))):
			return dieMontages;
		default:
			TArray<TObjectPtr<UAnimMontage>> null;
			return null;
	}
}

