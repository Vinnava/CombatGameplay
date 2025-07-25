// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionTraceANS.h"

#include "Gameplay/Actors/Equippables/Base/BaseWeapon.h"
#include "Gameplay/Components/CombatComponent.h"

#if WITH_EDITOR
FLinearColor UCollisionTraceANS::GetEditorColor()
{
	return FLinearColor::Red;
}
#endif

void UCollisionTraceANS::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration,
                                    const FAnimNotifyEventReference& eventReference)
{
	UCombatComponent* combatCompRef = meshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
	if (!combatCompRef) return;
	combatCompRef->GetMainWeapon()->ActivateCollision();
}

void UCollisionTraceANS::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation,
	const FAnimNotifyEventReference& eventReference)
{
	UCombatComponent* combatCompRef = meshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
	if (!combatCompRef) return;
	combatCompRef->GetMainWeapon()->DeactivateCollision();
}
