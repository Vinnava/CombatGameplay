// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEquippable.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(GPLogBaseEquippable);

// Sets default values
ABaseEquippable::ABaseEquippable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	itemMesh->SetupAttachment(RootComponent);
	itemMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	itemMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	itemMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	itemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

bool ABaseEquippable::AttachActorToSocket(FName socketName)
{
	if (TObjectPtr<ACharacter> attachActor = Cast<ACharacter>(GetOwner()))
	{
		if (TObjectPtr<USkeletalMeshComponent> characterMesh = attachActor->GetMesh())
		{
			bool bIsAttached = itemMesh->AttachToComponent(characterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, socketName);
			UE_LOG(GPLogBaseEquippable, Log, TEXT("[%s] Attached to Socket: %s"), *GetName(), *socketName.ToString());
			return bIsAttached;
		}
		else UE_LOG(GPLogBaseEquippable, Warning, TEXT("[%s] [AttachActorToSocket] characterMesh is Null"), *GetName());
	}
	UE_LOG(GPLogBaseEquippable, Warning, TEXT("[%s] [AttachActorToSocket] AttachActor is Null"), *GetName());
	return false;
}

void ABaseEquippable::OnEquipped()
{
	bIsEquipped = AttachActorToSocket(attachSocketName);
	
	UE_LOG(GPLogBaseEquippable, Log, TEXT("[%s] Equipped State: %s"), *GetName(), bIsEquipped ? TEXT("True") : TEXT("False"));
}

void ABaseEquippable::OnUnequipped()
{
	if (bIsEquipped)
	{
		bIsEquipped = false;
	}
}

#pragma region GameplayTagInterface
const FGameplayTag& ABaseEquippable::GetOwnedGameplayTag() const
{
	return ownedGameplayTag;
}

bool ABaseEquippable::HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck)
{
	return tagsToCheck.HasTag(ownedGameplayTag);
}
#pragma endregion GameplayTagInterface
