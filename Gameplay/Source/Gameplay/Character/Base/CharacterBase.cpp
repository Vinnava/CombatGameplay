// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharactermovementComponent.h"
#include "GameFramework/Controller.h"
#include "MotionWarpingComponent.h"
#include "Engine/DamageEvents.h"
#include "Gameplay/Actors/Camera/CameraShake/AttackCameraShake.h"
#include "Gameplay/Actors/Equippables/Base/BaseWeapon.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Components/EquipmentComponent.h"
#include "Gameplay/Components/StateManagerComponent.h"
#include "Gameplay/Components/StatsComponent.h"
#include "Gameplay/Data/GameplayData.h"
#include "Gameplay/Data/GameplayTagLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogCharacterBase, Log, All);


ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	//Creating Components
	statsComp = CreateDefaultSubobject<UStatsComponent>("StatsComponent");
	stateManagerComp = CreateDefaultSubobject<UStateManagerComponent>("StateManagerComponent");
	combatComp = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	equipComp = CreateDefaultSubobject<UEquipmentComponent>("EquipmentComponent");
	motionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}


void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (statsComp && stateManagerComp)
	{
		statsComp->OnHealthChanged.AddDynamic(this, &ACharacterBase::OnHealthChanged);
		stateManagerComp->OnCharacterStateBegin.AddUObject(this, &ACharacterBase::OnCharacterStateBegin);
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PostInitializeComponents] statsComp or stateManagerComp is null"), *GetName());
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (equipComp)
	{
		equipComp->InitializeEquipment();
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Equipment initialized"), *GetName());
	}
	else UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [BeginPlay] Cannot initialize equipment: equipComp is null"), *GetName());
}

bool ACharacterBase::CanPerformAttack() const
{
	if (!stateManagerComp || !combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [CanPerformAttack] combatComp or stateManagerComp is null"), *GetName());
		return false;
	}

	// Check for blocking states
	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(GameplayTags::State::Dead());
	statesToCheck.AddTag(GameplayTags::State::Dodging());
	statesToCheck.AddTag(GameplayTags::State::Disabled());
	statesToCheck.AddTag(GameplayTags::State::GeneralAction());
	statesToCheck.AddTag(GameplayTags::State::Attacking());
	
	return !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck) && combatComp->IsCombatEnabled();
}

void ACharacterBase::Attack()
{
	if (!CanPerformAttack())
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [Attack] Cannot perform attack - CanPerformAttack returns False"), *GetName());
		return;
	}

	if (!combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [Attack] combatComp is null"), *GetName());
		return;
	}

	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Performing light attack"), *GetName());
	PerformAttack(GameplayTags::Action::LightAttack(), combatComp->attackCount, false, false, 1.0f);
}

bool ACharacterBase::CanPerformDodge() const
{
	if (!stateManagerComp || !combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [CanPerformDodge] stateManagerComp or combatComp is null"), *GetName());
		return false;
	}
	
	// Check for blocking states
	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(GameplayTags::State::Dead());
	statesToCheck.AddTag(GameplayTags::State::Dodging());
	statesToCheck.AddTag(GameplayTags::State::Disabled());
	statesToCheck.AddTag(GameplayTags::State::GeneralAction());
	
	bool bCanDodge = !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck) && 
					 combatComp->IsCombatEnabled() && 
					 !GetCharacterMovement()->IsFalling();
	
	return bCanDodge;
}

void ACharacterBase::SetMovementSpeedMode(EMovementSpeedMode newMovementSpeedMode)
{
	if (newMovementSpeedMode == movementSpeedMode) 
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Movement speed mode unchanged: %d"), *GetName(), (int32)movementSpeedMode);
		return;
	}

	UCharacterMovementComponent* movementComp = GetCharacterMovement();
	if (!movementComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [SetMovementSpeedMode] CharacterMovementComponent is null"), *GetName());
		return;
	}

	EMovementSpeedMode previousMode = movementSpeedMode;
	movementSpeedMode = newMovementSpeedMode;
	
	float newSpeed;
	switch (movementSpeedMode)
	{
	case EMovementSpeedMode::Walking:
		newSpeed = maxWalkSpeed;
		movementComp->MaxWalkSpeed = newSpeed;
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Movement speed set to Walking: %.2f"), *GetName(), newSpeed);
		break;
	case EMovementSpeedMode::Jogging:
		newSpeed = maxJogSpeed;
		movementComp->MaxWalkSpeed = newSpeed;
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Movement speed set to Jogging: %.2f"), *GetName(), newSpeed);
		break;
	case EMovementSpeedMode::Sprinting:
		newSpeed = maxSprintSpeed;
		movementComp->MaxWalkSpeed = newSpeed;
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Movement speed set to Sprinting: %.2f"), *GetName(), newSpeed);
		break;
	default:
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [SetMovementSpeedMode] Invalid movement speed mode: %d"), *GetName(), (int32)movementSpeedMode);
		movementSpeedMode = previousMode; // Revert to previous mode
		break;
	}
}

EMovementSpeedMode ACharacterBase::GetMovementSpeedMode() const
{
	return movementSpeedMode;
}

EHitDirection ACharacterBase::UpdateAndGetHitDirection(FVector hitLocation) 
{
	// Get direction vectors
	FVector actorForward = GetActorForwardVector();
	FVector actorRight = GetActorRightVector();
	FVector toHit = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), hitLocation);

	// Calculate dot products for direction determination
	float forwardDot = UKismetMathLibrary::Dot_VectorVector(actorForward, toHit);
	float rightDot = UKismetMathLibrary::Dot_VectorVector(actorRight, toHit);

	// Determine hit direction based on dot products
	if (forwardDot >= 0.3f)
	{
		hitDirection = EHitDirection::Front;
	}
	else if (forwardDot <= -0.4f)
	{
		hitDirection = EHitDirection::Back;
	}
	else
	{
		hitDirection = (rightDot > 0.0f) ? EHitDirection::Right : EHitDirection::Left;
	}
	return hitDirection;
}

FRotator ACharacterBase::GetDesiredRotation() const
{
	FVector InputVector = GetLastMovementInputVector();

	if (!InputVector.IsNearlyZero(0.001f))
	{
		// Return rotation based on input vector
		FRotator inputRotation = InputVector.Rotation();
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Desired rotation from input: %s"), *GetName(), *inputRotation.ToString());
		return inputRotation;
	}
	else
	{
		// No input → return current actor rotation
		FRotator currentRotation = GetActorRotation();
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Desired rotation (no input): %s"), *GetName(), *currentRotation.ToString());
		return currentRotation;
	}
}

float ACharacterBase::TakeDamage(float damage, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Taking damage: %.2f from %s"), *GetName(), damage, 
		   damageCauser ? *damageCauser->GetName() : TEXT("Unknown"));

	float actualDamage = Super::TakeDamage(damage, damageEvent, eventInstigator, damageCauser);

	if (!CanReciveDamage())
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [TakeDamage] Cannot receive damage in current state"), *GetName());
		return 0.0f;
	}

	// Extract hit location from damage event
	FVector hitLocation = GetActorLocation(); // Default fallback
	if (damageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(damageEvent);
		hitLocation = PointDamageEvent.HitInfo.ImpactPoint;
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Hit location extracted: %s"), *GetName(), *hitLocation.ToString());
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [TakeDamage] Damage event is not PointDamageEvent, using actor location"), *GetName());

	UpdateAndGetHitDirection(hitLocation);

	// Perform hit reaction and apply damage with time dilation effect
	if (bool bCanDamage = PerformHitReaction(hitLocation, damage))
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Hit reaction performed, applying time dilation"), *GetName());
		
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
		
		float delayTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 1.2f;
		FTimerHandle timerHandleUpdateDamage;
		TWeakObjectPtr<ACharacterBase> weakThis = this;
		
		GetWorld()->GetTimerManager().SetTimer(timerHandleUpdateDamage,
			[weakThis, bCanDamage, damage, eventInstigator, hitLocation]()
			{
				if (weakThis.IsValid())
				{
					weakThis->ApplyDamage(bCanDamage, damage, eventInstigator, hitLocation);
				}
				else UE_LOG(GPLogCharacterBase, Warning, TEXT("Character destroyed before damage could be applied"));
			}, delayTime, false);
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [TakeDamage] Hit reaction failed"), *GetName());
	
	return actualDamage;
}

void ACharacterBase::ApplyDamage(bool bCanDamage, float damage, AController* instigatorController, FVector hitLocation)
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Applying damage: %.2f (CanDamage: %s)"), *GetName(), damage, 
		   bCanDamage ? TEXT("true") : TEXT("false"));

	// Reset time dilation
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	
	// Apply camera shake effect
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	if (playerController && playerController->PlayerCameraManager)
	{
		playerController->PlayerCameraManager->StartCameraShake(UAttackCameraShake::StaticClass(), 0.5f, 
			ECameraShakePlaySpace::CameraLocal, FRotator::ZeroRotator);
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Camera shake applied"), *GetName());
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [ApplyDamage] Could not apply camera shake - PlayerController or CameraManager is null"), *GetName());
	
	if (!bCanDamage)
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Damage application blocked"), *GetName());
		return;
	}

	// Validate instigator controller
	if (!instigatorController)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [ApplyDamage] instigatorController is null"), *GetName());
		return;
	}

	// Report damage to AI system
	if (APawn* instigatorPawn = instigatorController->GetPawn())
	{
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, instigatorPawn, damage, hitLocation, hitLocation, TEXT("None"));
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Damage reported to AI system"), *GetName());
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [ApplyDamage] Could not report damage - instigator pawn is null"), *GetName());
	
	// Apply health change
	if (!statsComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [ApplyDamage] statsComp is null"), *GetName());
		return;
	}

	if (!statsComp->ApplyHealthChange(damage * -1.0f))
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Health depleted, setting character to dead state"), *GetName());
		
		if (stateManagerComp)
		{
			stateManagerComp->SetCurrentState(GameplayTags::State::Dead());
		}
		else UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [ApplyDamage] Cannot set dead state - stateManagerComp is null"), *GetName());
	}
}

void ACharacterBase::OnHealthChanged(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta)
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Health changed: %.2f (Delta: %.2f)"), *GetName(), NewHealth, Delta);

	// Disable input when character dies
	if (NewHealth <= 0.0f && Delta <= 0.0f)
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Character died, disabling input"), *GetName());

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			DisableInput(PlayerController);
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Input disabled for player controller"), *GetName());
		}
		else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [OnHealthChanged] Could not disable input - not a player controller"), *GetName());
	}
}

void ACharacterBase::OnCharacterStateBegin(FGameplayTag characterState)
{
	if (characterState.MatchesTagExact(GameplayTags::State::Dead()))
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Handling death state"), *GetName());
		
		FPerformDeath performDeath = PerformDeath();
		
		if (performDeath.duration > 0.0f)
		{
			float delayTime = performDeath.duration + 4.0f;
			TWeakObjectPtr<ACharacterBase> weakThis = this;
			FTimerHandle timerHandleDestroyDeadActors;
			FTimerDelegate timerDelegateDestroyDeadActors;

			// Schedule destruction of attached actors and self
			timerDelegateDestroyDeadActors.BindLambda([weakThis, performDeath]()
			{
				UE_LOG(GPLogCharacterBase, Log, TEXT("Destroying dead character and attached actors"));
				
				for (TObjectPtr<AActor> actor : performDeath.actorsToDestory)
				{
					if (actor)
					{
						UE_LOG(GPLogCharacterBase, Log, TEXT("Destroying attached actor: %s"), *actor->GetName());
						actor->Destroy();
					}
				}
				
				if (weakThis.IsValid())
				{
					UE_LOG(GPLogCharacterBase, Log, TEXT("Destroying character: %s"), *weakThis->GetName());
					weakThis->Destroy();
				}
			});
			
			if (timerHandleDestroyDeadActors.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(timerHandleDestroyDeadActors);
			}
			GetWorld()->GetTimerManager().SetTimer(timerHandleDestroyDeadActors, timerDelegateDestroyDeadActors, delayTime, false);
			
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Death cleanup scheduled for %.2f seconds"), *GetName(), delayTime);
		}
		else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [OnCharacterStateBegin] Death duration is 0, immediate cleanup may be needed"), *GetName());
	}
}

void ACharacterBase::EnableRagdoll() const
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None, 0);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"), true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, true, true);
}


#pragma region IGameplayTagInterface

const FGameplayTag& ACharacterBase::GetOwnedGameplayTag() const
{
	return ownedGameplayTag;
}

bool ACharacterBase::HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck)
{
	return tagsToCheck.HasTag(ownedGameplayTag);
}
#pragma endregion IGameplayTagInterface


#pragma region ICombatInterface

void ACharacterBase::ContinueAttack()
{
	if (!combatComp || !stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [ContinueAttack] combatComp or stateManagerComp is null"), *GetName());
		return;
	}

	if (combatComp->bIsAttackSaved)
	{
		combatComp->bIsAttackSaved = false;
		
		if (stateManagerComp->GetCurrentState() == GameplayTags::State::Attacking())
		{
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] In attacking state, resetting and continuing"), *GetName());
			stateManagerComp->ResetState();
			Attack();
		}
		else
		{
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Not in attacking state, starting new attack"), *GetName());
			Attack();
		}
	}
	else
	{
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] No saved attack, enabling continue flag"), *GetName());
		combatComp->bCanContinueAttack = true;
	}
}

void ACharacterBase::ResetCombat()
{
	if (!combatComp || !stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [ResetCombat] combatComp or stateManagerComp is null"), *GetName());
		return;
	}
	
	combatComp->ResetCombat();
	stateManagerComp->ResetState();
	stateManagerComp->ResetAction();
}

bool ACharacterBase::CanReciveDamage()
{
if (!stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [CanReciveDamage] stateManagerComp is null"), *GetName());
		return false;
	}

	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(GameplayTags::State::Dead());
	statesToCheck.AddTag(GameplayTags::State::Dodging());
	
	bool bCanReceiveDamage = !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck);
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] [CanReciveDamage] Result: %s"), *GetName(), bCanReceiveDamage ? TEXT("true") : TEXT("false"));
	
	return bCanReceiveDamage;
}

FPerformDeath ACharacterBase::PerformDeath()
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Performing death sequence"), *GetName());

	FPerformDeath returnPerformDeath
	{
		{},
		0.0f
	};

	// Validate essential components
	if (!combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformDeath] combatComp is null"), *GetName());
		return returnPerformDeath;
	}

	ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();
	if (!mainWeapon)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformDeath] No main weapon found"), *GetName());
		return returnPerformDeath;
	}

	if (!stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformDeath] stateManagerComp is null"), *GetName());
		return returnPerformDeath;
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	// Set death action
	stateManagerComp->SetCurrentAction(GameplayTags::Action::Die());

	// Collect equipment for destruction
	if (equipComp)
	{
		TArray<ABaseEquippable*> currentEquipments = equipComp->GetCurrentEquipments();
		for (ABaseEquippable* currentEquipment : currentEquipments)
		{
			if (currentEquipment)
			{
				returnPerformDeath.actorsToDestory.Add(currentEquipment);
				UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Added equipment to destruction list: %s"), *GetName(), *currentEquipment->GetName());
			}
		}
	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformDeath] equipComp is null, cannot collect equipment"), *GetName());
	
	// Handle weapon physics and destruction
	mainWeapon->SimulateWeaponPhysics();
	returnPerformDeath.actorsToDestory.Add(mainWeapon);
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Main weapon physics enabled and added to destruction list"), *GetName());

	// Play death animation
	TArray<UAnimMontage*> actionMontageArray = mainWeapon->GetActionMontages(GameplayTags::Action::Die());
	if (actionMontageArray.IsEmpty())
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformDeath] No death montages found"), *GetName());
	}
	else
	{
		EnableRagdoll();
		
		int32 randomIndex = FMath::RandRange(0, actionMontageArray.Num() - 1);
		UAnimMontage* selectedMontage = actionMontageArray[randomIndex];
		
		if (selectedMontage)
		{
			USkeletalMeshComponent* MeshComp = GetMesh();
			if (MeshComp && MeshComp->GetAnimInstance())
			{
				float animDuration = MeshComp->GetAnimInstance()->Montage_Play(selectedMontage);
				returnPerformDeath.duration = animDuration;
				UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Death montage playing: %s (Duration: %.2f)"), *GetName(), *selectedMontage->GetName(), animDuration);
			}
			else
			{
				UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformDeath] Cannot play death montage - mesh or anim instance is null"), *GetName());
			}
		}
		else
		{
			UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformDeath] Selected death montage is null"), *GetName());
		}
	}

	// Handle AI controller cleanup
	if (AAIController* aiController = Cast<AAIController>(GetController()))
	{
		if (aiController->BrainComponent)
		{
			aiController->BrainComponent->StopLogic("Dead");
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] AI logic stopped"), *GetName());
		}
		else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformDeath] AI controller has no brain component"), *GetName());
	}
	else UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] [PerformDeath] Controller is not an AI controller"), *GetName());

	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Death sequence completed (Actors to destroy: %d)"), 
		   *GetName(), returnPerformDeath.actorsToDestory.Num());

	return returnPerformDeath;
}

bool ACharacterBase::PerformHitReaction(FVector hitLocation, float damage)
{
UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Performing hit reaction (Damage: %.2f, Location: %s)"), *GetName(), damage, *hitLocation.ToString());

	if (!stateManagerComp || !combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformHitReaction] stateManagerComp or combatComp is null"), *GetName());
		return false;
	}

	// Set disabled state during hit reaction
	stateManagerComp->SetCurrentAction(GameplayTags::State::Disabled());

	ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();
	if (!mainWeapon)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformHitReaction] No main weapon found"), *GetName());
		return false;
	}

	// Select appropriate hit animation based on direction
	UAnimMontage* hitAnimMontage = nullptr;
	FString directionString;
	
	switch (hitDirection)
	{
	case EHitDirection::Front:
		hitAnimMontage = mainWeapon->hitMontage_F;
		directionString = TEXT("Front");
		break;
	case EHitDirection::Back:
		hitAnimMontage = mainWeapon->hitMontage_B;
		directionString = TEXT("Back");
		break;
	case EHitDirection::Left:
		hitAnimMontage = mainWeapon->hitMontage_L;
		directionString = TEXT("Left");
		break;
	case EHitDirection::Right:
		hitAnimMontage = mainWeapon->hitMontage_R;
		directionString = TEXT("Right");
		break;
	default:
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformHitReaction] Invalid hit direction: %d"), *GetName(), (int32)hitDirection);
		break;
	}

	if (hitAnimMontage)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp && MeshComp->GetAnimInstance())
		{
			float playedDuration = MeshComp->GetAnimInstance()->Montage_Play(hitAnimMontage, 1.0f,
				EMontagePlayReturnType::Duration, 0.0f, true);
			
			UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Hit reaction montage playing: %s direction (Duration: %.2f)"), 
				   *GetName(), *directionString, playedDuration);
			return true;
		}
		else UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformHitReaction] Cannot play hit montage - mesh or anim instance is null"), *GetName());

	}
	else UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformHitReaction] No hit montage found for %s direction"), *GetName(), *directionString);
	
	return false;
}

FPerformAction ACharacterBase::PerformAction(FGameplayTag characterState, FGameplayTag characterAction,
	int32 montageIndex, bool bRandomIndex)
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Performing action: %s (State: %s, Index: %d, Random: %s)"), 
		   *GetName(), *characterAction.ToString(), *characterState.ToString(), montageIndex, bRandomIndex ? TEXT("true") : TEXT("false"));

	FPerformAction returnPerformAction
	{
		false,
		0.0f
	};

	// Validate essential components
	if (!combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAction] combatComp is null"), *GetName());
		return returnPerformAction;
	}

	ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();
	if (!mainWeapon)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformAction] No main weapon found"), *GetName());
		return returnPerformAction;
	}

	if (!stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAction] stateManagerComp is null"), *GetName());
		return returnPerformAction;
	}

	// Get action montages
	TArray<UAnimMontage*> actionMontageArray = mainWeapon->GetActionMontages(characterAction);
	if (actionMontageArray.Num() == 0)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformAction] No montages found for action: %s"), *GetName(), *characterAction.ToString());
		return returnPerformAction;
	}

	// Select montage index
	int32 randomIndex = FMath::RandRange(0, actionMontageArray.Num() - 1);
	int32 selectedIndex = bRandomIndex ? randomIndex : montageIndex;
	
	if (!actionMontageArray.IsValidIndex(selectedIndex))
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAction] Invalid montage index: %d (Array size: %d)"), 
			   *GetName(), selectedIndex, actionMontageArray.Num());
		return returnPerformAction;
	}

	UAnimMontage* actionMontage = actionMontageArray[selectedIndex];
	if (!actionMontage)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAction] Selected montage is null at index: %d"), *GetName(), selectedIndex);
		return returnPerformAction;
	}

	// Set states and play animation
	stateManagerComp->SetCurrentState(characterState);
	stateManagerComp->SetCurrentAction(characterAction);

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp && MeshComp->GetAnimInstance())
	{
		float actionDuration = MeshComp->GetAnimInstance()->Montage_Play(actionMontage, 1.0f,
			EMontagePlayReturnType::Duration, 0.0f, true);
		
		returnPerformAction.actionDuration = actionDuration;
		returnPerformAction.bSuccess = true;
		
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Action performed successfully: %s (Duration: %.2f)"), 
			   *GetName(), *actionMontage->GetName(), actionDuration);
	}
	else UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAction] Cannot play action montage - mesh or anim instance is null"), *GetName());

	return returnPerformAction;
}

FPerformAttack ACharacterBase::PerformAttack(FGameplayTag attackType, int32 attackIndex, bool bRandomIndex,
												bool bIsCalledByAI, float playRate)
{
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Performing attack: %s (Index: %d, Random: %s, AI: %s, Rate: %.2f)"), 
		   *GetName(), *attackType.ToString(), attackIndex, bRandomIndex ? TEXT("true") : TEXT("false"), 
		   bIsCalledByAI ? TEXT("true") : TEXT("false"), playRate);

	FPerformAttack returnPerformAttack
	{
		false,
		0.0f
	};

	// Validate essential components
	if (!combatComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAttack] combatComp is null"), *GetName());
		return returnPerformAttack;
	}

	ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();
	if (!mainWeapon)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformAttack] No main weapon found"), *GetName());
		return returnPerformAttack;
	}

	if (!stateManagerComp)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAttack] stateManagerComp is null"), *GetName());
		return returnPerformAttack;
	}

	// Get attack montages
	TArray<UAnimMontage*> attackMontageArray = mainWeapon->GetActionMontages(attackType);
	if (attackMontageArray.Num() == 0)
	{
		UE_LOG(GPLogCharacterBase, Warning, TEXT("[%s] [PerformAttack] No attack montages found for type: %s"), *GetName(), *attackType.ToString());
		return returnPerformAttack;
	}

	// Select montage index
	int32 randomIndex = FMath::RandRange(0, attackMontageArray.Num() - 1);
	int32 selectedIndex = bRandomIndex ? randomIndex : attackIndex;
	
	if (!attackMontageArray.IsValidIndex(selectedIndex))
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAttack] Invalid attack index: %d (Array size: %d)"), 
			   *GetName(), selectedIndex, attackMontageArray.Num());
		return returnPerformAttack;
	}

	UAnimMontage* attackMontage = attackMontageArray[selectedIndex];
	if (!attackMontage)
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAttack] Selected attack montage is null at index: %d"), *GetName(), selectedIndex);
		return returnPerformAttack;
	}

	// Set attack states
	stateManagerComp->SetCurrentState(GameplayTags::State::Attacking());
	stateManagerComp->SetCurrentAction(attackType);

	// Perform target detection for positioning
	FHitResult hitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	bool bHitTarget = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(), GetActorLocation(),
		40.0f, objectTypes, false, {}, EDrawDebugTrace::None, hitResult, true);

	// Calculate positioning transform
	//FTransform targetTransform;
	FVector targetLocation = GetActorLocation(); // Default to current location
	FRotator targetRotation = GetActorRotation(); // Default to current rotation

	if (bHitTarget && hitResult.GetActor())
	{
		AActor* hitActor = hitResult.GetActor();
		FVector directionToTarget = UKismetMathLibrary::GetForwardVector(
			UKismetMathLibrary::FindLookAtRotation(hitActor->GetActorLocation(), GetActorLocation()));
		
		targetLocation = directionToTarget * 200.0f + hitActor->GetActorLocation();
		targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), hitActor->GetActorLocation());

		motionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(attackWarpTargetName, targetLocation, targetRotation);
		
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Target detected for attack positioning: %s"), *GetName(), *hitActor->GetName());
	}
	else UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] No target detected, using current transform"), *GetName());


	//targetTransform.SetLocation(targetLocation);
	//targetTransform.SetRotation(targetRotation.Quaternion());

	// Play attack animation
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp && MeshComp->GetAnimInstance())
	{
		float attackDuration = MeshComp->GetAnimInstance()->Montage_Play(attackMontage, playRate,
			EMontagePlayReturnType::Duration, 0.0f, true);
		
		returnPerformAttack.attackDuration = attackDuration;
		returnPerformAttack.bSuccess = true;
		
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Attack montage playing: %s (Duration: %.2f)"), 
			   *GetName(), *attackMontage->GetName(), attackDuration);
	}
	else
	{
		UE_LOG(GPLogCharacterBase, Error, TEXT("[%s] [PerformAttack] Cannot play attack montage - mesh or anim instance is null"), *GetName());
		return returnPerformAttack;
	}

	// Update attack count and cycle management
	combatComp->attackCount++;
	int32 currentAttackIndex = combatComp->attackCount;
	int32 lastMontageIndex = attackMontageArray.Num() - 1;
	
	if (currentAttackIndex > lastMontageIndex)
	{
		combatComp->attackCount = 0;
		UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Attack count reset to 0 after reaching max"), *GetName());
	}
	UE_LOG(GPLogCharacterBase, Log, TEXT("[%s] Attack performed successfully (Count: %d/%d)"), *GetName(), attackIndex, lastMontageIndex);
	return returnPerformAttack;
}
#pragma endregion ICombatInterface

