// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetCombatAN.h"

#include "Gameplay/Interface/CombatInterface.h"

void UResetCombatAN::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation,
                            const FAnimNotifyEventReference& eventReference)
{
	if (ICombatInterface* combatInterface = Cast<ICombatInterface>(meshComp->GetOwner()))
	{
		combatInterface->ResetCombat();
	}
}
