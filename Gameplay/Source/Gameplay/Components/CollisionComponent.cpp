// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionComponent.h"

#include "CollisionShape.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Gameplay/Character/Base/CharacterBase.h"


// Sets default values for this component's properties
UCollisionComponent::UCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled)
	{
		CollisionTrace();
	}
}

//Activates the collision functionality for the UCollisionComponent.
void UCollisionComponent::ActivateCollision()
{
	ClearHitActors();
	bIsCollisionEnabled = true;
}

//Deactivates the collision functionality for the UCollisionComponent.
void UCollisionComponent::DeactivateCollision()
{
	bIsCollisionEnabled = false;
}

//Clears the list of actors that have already been hit by the collision trace.
void UCollisionComponent::ClearHitActors()
{
	alreadyHitActors.Empty();
}

//Sets the collision mesh component for the collision system.
void UCollisionComponent::SetCollisionMeshComponent(TObjectPtr<UPrimitiveComponent> collisionMesh)
{
	collisionMeshComp = collisionMesh;
}

/**
 * Performs a collision trace between two specified socket locations and processes the results.
 * 
 * If valid hit actors are detected, the results are broadcasted via the `onHitCollision` delegate/event
 * with the last hit result for subscribing systems to respond to.
 */
void UCollisionComponent::CollisionTrace()
{
	FVector startLoc = collisionMeshComp->GetSocketLocation(startSocketName);
	FVector endLoc = collisionMeshComp->GetSocketLocation(endSocketName);
	float radius = traceRadius;

	// Define the collision shape as a sphere
	FCollisionShape sphereShape = FCollisionShape::MakeSphere(traceRadius);

	// Result array
	TArray<FHitResult> hitResults;

	//Actors to ignore
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActors(actorsToIgnore);

	// Object types to trace
	FCollisionObjectQueryParams objectParams;
	for (ECollisionChannel Channel : traceObjectTypes)
	{
		objectParams.AddObjectTypesToQuery(Channel);
	}

	// Perform the multi-sphere trace
	bool bHit = GetWorld()->SweepMultiByObjectType(
		hitResults,
		startLoc,
		endLoc,
		FQuat::Identity,
		objectParams,
		sphereShape,
		queryParams
	);

	// Process results
	if (bHit)
	{
		for (const FHitResult& hitResult : hitResults)
		{
			AActor* hitActor = hitResult.GetActor();
			//ACharacterBase* hitActor = Cast<ACharacterBase>(hitResult.GetActor());
			if (hitActor)
			{
				if (const IGameplayTagInterface* hitActorTagInterface = Cast<IGameplayTagInterface>(hitActor))
				{
					const FGameplayTag& hitActorTag = hitActorTagInterface->GetOwnedGameplayTag();
					if (!ignoreGameplayTags.HasTag(hitActorTag) && !alreadyHitActors.Contains(hitActor))
					{
						lastHitResult = hitResult;
						lastHitActor = hitActor;
						alreadyHitActors.Add(hitActor);
						onHitCollision.Broadcast(lastHitResult);
					}
				}
			}
		}
	}
}


