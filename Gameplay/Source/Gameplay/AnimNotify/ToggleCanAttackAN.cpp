// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleCanAttackAN.h"

#include "Gameplay/AI/Controller/AICEnemy.h"

void UToggleCanAttackAN::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation,
                                const FAnimNotifyEventReference& eventReference)
{
	if (APawn* pawn = Cast<APawn>(meshComp->GetOwner()))
	{
		if (AAICEnemy* enemyController = Cast<AAICEnemy>(pawn->GetController()))
		{
			enemyController->ToggleCanAttack(bCanAttack);
		}
	}
}
