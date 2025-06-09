// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHit, FHitResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAY_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollisionComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private://Variables
	TObjectPtr<UPrimitiveComponent> collisionMeshComp;
	bool bIsCollisionEnabled{false};
	FHitResult lastHitResult;
	TObjectPtr<AActor> lastHitActor;
	TArray<TObjectPtr<AActor>> alreadyHitActors;
	
protected://Variables
#pragma region 	Initialization Variables
	//Initialization
	FName startSocketName;
	FName endSocketName;
	float traceRadius;
	EObjectTypeQuery traceObjectType;
	TArray<TObjectPtr<AActor>> actorsToIgnore;
	EDrawDebugTrace::Type drawDebugType;
	FGameplayTagContainer ignoreGameplayTags;
#pragma endregion 
	
public://Variables
	FOnHit onHitCollision;
	
private://Functions

	
protected://Functions

	
public://Functions
	void Initialize(const FName& inStartSocketName,
					const FName& inEndSocketName,
					float inTraceRadius,
					EObjectTypeQuery inTraceObjectType,
					const TArray<TObjectPtr<AActor>>& inActorsToIgnore,
					EDrawDebugTrace::Type inDrawDebugType,
					const FGameplayTagContainer& inIgnoreGameplayTags);
	void ActivateCollision();
	void DeactivateCollision();
	void ClearHitActors();
	void SetCollisionMeshComponent(TObjectPtr<UPrimitiveComponent> collisionMesh);
	void CollisionTrace();
//Getters
	bool IsCollisionEnabled() const {return bIsCollisionEnabled;}
	TArray<TObjectPtr<AActor>> GetAlreadyHitActors() const {return alreadyHitActors;}
	TObjectPtr<AActor>GetLastHitActor() const {return lastHitActor;}
	TObjectPtr<UPrimitiveComponent> GetCollisionMeshComp() const {return collisionMeshComp;}
	TArray<TObjectPtr<AActor>> GetActorsToIgnore() const {return actorsToIgnore;}
//Helpers
	void AddIgnoreActor(AActor* actorToIgnore) { actorsToIgnore.AddUnique(actorToIgnore); }
	void RemoveIgnoreActor(AActor* actorToIgnore) { actorsToIgnore.Remove(actorToIgnore); }
};
