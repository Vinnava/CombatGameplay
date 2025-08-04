// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionComponent.h"

#include "CollisionShape.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Gameplay/Character/Base/CharacterBase.h"


UCollisionComponent::UCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled)
	{
		CollisionTrace();
	}
}

void UCollisionComponent::ActivateCollision()
{
	ClearHitActors();
	bIsCollisionEnabled = true;
}

void UCollisionComponent::DeactivateCollision()
{
	bIsCollisionEnabled = false;
}

void UCollisionComponent::ClearHitActors()
{
	alreadyHitActors.Empty();
}

void UCollisionComponent::SetCollisionMeshComponent(TObjectPtr<UPrimitiveComponent> collisionMesh)
{
	collisionMeshComp = collisionMesh;
}

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
	bool bHit = GetWorld()->SweepMultiByObjectType(hitResults,
													startLoc,
													endLoc,
													FQuat::Identity,
													objectParams,
													sphereShape,
													queryParams);

	// Process results
	if (bHit)
	{
		for (const FHitResult& hitResult : hitResults)
		{
			if (AActor* hitActor = hitResult.GetActor())
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


