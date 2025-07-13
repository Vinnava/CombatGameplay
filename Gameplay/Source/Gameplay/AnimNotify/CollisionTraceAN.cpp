// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionTraceAN.h"

#include "Gameplay/Actors/Equippables/Base/BaseWeapon.h"
#include "Gameplay/Components/CombatComponent.h"

void UCollisionTraceAN::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration,
                                    const FAnimNotifyEventReference& eventReference)
{
	UCombatComponent* combatCompRef = meshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
	if (!combatCompRef) return;
	combatCompRef->GetMainWeapon()->ActivateCollision();
}

void UCollisionTraceAN::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation,
	const FAnimNotifyEventReference& eventReference)
{
	UCombatComponent* combatCompRef = meshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
	if (!combatCompRef) return;
	combatCompRef->GetMainWeapon()->DeactivateCollision();
}
