// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Gameplay/Components/CollisionComponent.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Data/GameplayTagLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogBaseWeapon, Log, All)

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the collision component
	collisionComp = CreateDefaultSubobject<UCollisionComponent>("CollisionComp");
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (collisionComp)
	{
		collisionComp->onHitCollision.AddUObject(this, &ABaseWeapon::OnHit);
		UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] CollisionComponent OnHitCollision Bind Successful"), *GetName());
	}
	else UE_LOG(GPLogBaseWeapon, Error, TEXT("[%s] [BeginPlay] CollisionComponent OnHitCollision Bind NotSuccessful"), *GetName());
}

void ABaseWeapon::OnEquipped()
{
	Super::OnEquipped();
	
	combatComp = GetOwner()->FindComponentByClass<UCombatComponent>();
	if (!combatComp)
	{
		UE_LOG(GPLogBaseWeapon, Error, TEXT("[%s] [OnEquipped] No CombatComponent found on Owner [%s]"), *GetName(), *GetOwner()->GetName());
		return;
	}

	combatComp->SetMainWeapon(this);
	ToggleCombat();  //NOTE: Temp
	UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Equipped to [%s]"), *GetName(), *GetOwner()->GetName());

	if (collisionComp)
	{
		collisionComp->SetCollisionMeshComponent(itemMesh);
		collisionComp->AddIgnoreActor(GetOwner());
	}
	else UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [OnEquipped] collisionComp is null"), *GetName());
}

void ABaseWeapon::OnHit(FHitResult hitResult)
{
	AActor* hitActor = hitResult.GetActor();
	if (!hitActor)
	{
		UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [OnHit] called with null actor"), *GetName());
		return;
	}

	UGameplayStatics::ApplyPointDamage(hitActor,
									GetDamage(),
									GetOwner()->GetActorForwardVector(),
									hitResult,GetInstigatorController(),
									this,
									UDamageType::StaticClass());

	UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Hit Actor: [%s] with %.1f damage"), *GetName(), *hitActor->GetName(), GetDamage());
}

void ABaseWeapon::SimulateWeaponPhysics()
{
	if (itemMesh)
	{
		itemMesh->SetCollisionProfileName("PhysicalActor", true);
		itemMesh->SetSimulatePhysics(true);
		UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Simulating Weapon Physics"), *GetName());
	}
	else UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [SimulateWeaponPhysics] itemMesh is null"), *GetName());
}

void ABaseWeapon::ActivateCollision()
{
	if (collisionComp)
	{
		collisionComp->ActivateCollision();
		UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Collision Activated"), *GetName());
	}
	else UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [ActivateCollision] Failed to activate collision: collisionComp is null"), *GetName());
}

void ABaseWeapon::DeactivateCollision()
{
	if (collisionComp)
	{
		collisionComp->DeactivateCollision();
		UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Collision Deactivated"), *GetName());
	}
	else UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [DeactivateCollision] Failed to deactivate collision: collisionComp is null "), *GetName());
}

void ABaseWeapon::ToggleCombat()
{
	if (combatComp)
	{
		combatComp->SetCombatEnabled(true);
		UE_LOG(GPLogBaseWeapon, Log, TEXT("[%s] Combat Toggled On"), *GetName());
	}
	else UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [ToggleCombat] Cannot toggle combat: combatComp is null"), *GetName());
}

const TArray<UAnimMontage*>& ABaseWeapon::GetActionMontages(FGameplayTag characterAction) const
{
	if (characterAction.MatchesTagExact(GameplayTags::Action::LightAttack()))
	{
		return lightAttackMontages;
	}
	else if (characterAction.MatchesTagExact(GameplayTags::Action::Dodge()))
	{
		return dodgeMontages;
	}
	else if (characterAction.MatchesTagExact(GameplayTags::Action::Die()))
	{
		return dieMontages;
	}
	else
	{
		UE_LOG(GPLogBaseWeapon, Warning, TEXT("[%s] [GetActionMontages] No matching montage found for tag: %s"), *GetName(), *characterAction.ToString());
		static const TArray<UAnimMontage*> EmptyMontages;
		return EmptyMontages;
	}
}

