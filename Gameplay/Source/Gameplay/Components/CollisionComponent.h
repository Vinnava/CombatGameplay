// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHit, const FHitResult&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

private://Variables
#pragma region 	Initialization Variables
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	FName startSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	FName endSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	float traceRadius{0.0f};
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	TArray<TEnumAsByte<ECollisionChannel>> traceObjectTypes;
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	TArray<TObjectPtr<AActor>> actorsToIgnore;
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	TEnumAsByte<EDrawDebugTrace::Type> drawDebugType{};
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	FGameplayTagContainer ignoreGameplayTags;
#pragma endregion
	
	TObjectPtr<UPrimitiveComponent> collisionMeshComp ;
	bool bIsCollisionEnabled{false};
	FHitResult lastHitResult;
	TObjectPtr<AActor> lastHitActor;
	TArray<TObjectPtr<AActor>> alreadyHitActors;

public://Variables
	FOnHit onHitCollision;
	
private://Functions

	
public://Functions
	// Sets default values for this component's properties
	UCollisionComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//Activates the collision functionality for the UCollisionComponent.
	void ActivateCollision();
	
	//Deactivates the collision functionality for the UCollisionComponent.
	void DeactivateCollision();
	
	//Clears the list of actors that have already been hit by the collision trace.
	void ClearHitActors();

	//Sets the collision mesh component for the collision system.
	void SetCollisionMeshComponent(TObjectPtr<UPrimitiveComponent> collisionMesh);

	//Performs a collision trace between two specified socket locations and processes the results.
	void CollisionTrace();
	
#pragma region Getters
	bool IsCollisionEnabled() const {return bIsCollisionEnabled;}
	TArray<TObjectPtr<AActor>> GetAlreadyHitActors() const {return alreadyHitActors;}
	TObjectPtr<AActor>GetLastHitActor() const {return lastHitActor;}
	TObjectPtr<UPrimitiveComponent> GetCollisionMeshComp() const {return collisionMeshComp;}
	TArray<TObjectPtr<AActor>> GetActorsToIgnore() const {return actorsToIgnore;}
#pragma endregion Getters
	
#pragma region Helper Functions
	void AddIgnoreActor(AActor* actorToIgnore) { actorsToIgnore.AddUnique(actorToIgnore); }
	void RemoveIgnoreActor(AActor* actorToIgnore) { actorsToIgnore.Remove(actorToIgnore); }
#pragma endregion Helper Functions	
};
