// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "MotionWarpingComponent.h"
#include "Chaos/Utilities.h"
#include "Engine/DamageEvents.h"
#include "Gameplay/Actors/Camera/CameraShake/AttackCameraShake.h"
#include "Gameplay/Actors/Equippables/Base/BaseWeapon.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Components/EquipmentComponent.h"
#include "Gameplay/Components/StateManagerComponent.h"
#include "Gameplay/Components/StatsComponent.h"
#include "Gameplay/Data/GameplayData.h"
#include "Gameplay/Data/GameplayTagData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY(LogBaseCharacter);


ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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
	combatComp = CreateDefaultSubobject<UCombatComponent>("NCombatComponent");
	equipComp = CreateDefaultSubobject<UEquipmentComponent>("NEquipmentComponent");
	motionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>("NMotionWarpingComponent");
}


void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	statsComp->OnHealthChanged.AddDynamic(this,&ACharacterBase::OnHealthChanged);
	stateManagerComp->OnCharacterStateBegin.AddUObject(this,&ACharacterBase::OnCharacterStateBegin);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	equipComp->InitializeEquipment();
}

bool ACharacterBase::CanPerformAttack() const
{
	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(deadStateTag);
	statesToCheck.AddTag(dodgingStateTag);
	statesToCheck.AddTag(disabledStateTag);
	statesToCheck.AddTag(generalActionStateTag);
	statesToCheck.AddTag(attackingStateTag);
	return !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck) && combatComp->bIsCombatEnabled;
}

void ACharacterBase::Attack()
{
	if (CanPerformAttack())
	{
		PerformAttack(lightAttackActionTag, combatComp->attackCount, false, false, 1.0f);
	}
}

bool ACharacterBase::CanPerformDodge() const
{
	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(deadStateTag);
	statesToCheck.AddTag(dodgingStateTag);
	statesToCheck.AddTag(generalActionStateTag);
	statesToCheck.AddTag(disabledStateTag);
	
	return !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck) && !(!combatComp->bIsCombatEnabled || GetCharacterMovement()->IsFalling());
}

void ACharacterBase::SetMovementSpeedMode(EMovementSpeedMode newMovementSpeedMode)
{
	if (newMovementSpeedMode!=movementSpeedMode) return;
	movementSpeedMode = newMovementSpeedMode;
	switch (movementSpeedMode)
	{
		case EMovementSpeedMode::Walking :
			GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed;
			break;
		case EMovementSpeedMode::Jogging :
			GetCharacterMovement()->MaxWalkSpeed = maxJogSpeed;
			break;
		case EMovementSpeedMode::Sprinting :
			GetCharacterMovement()->MaxWalkSpeed = maxSprintSpeed;
			break;
	}
}

EMovementSpeedMode ACharacterBase::GetMovementSpeedMode() const
{
	return movementSpeedMode;
}

EHitDirection ACharacterBase::UpdateAndGetHitDirection(FVector hitLocation) 
{
	FVector actorForward = GetActorForwardVector();
	FVector actorRight = GetActorRightVector();
	FVector toHit = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), hitLocation);

	float forwardDot = UKismetMathLibrary::Dot_VectorVector(actorForward, toHit);
	if (forwardDot >= 0.3f)
	{
		hitDirection = EHitDirection::Front;
		return hitDirection;
	}
	else if (forwardDot <= -0.4f)
	{
		hitDirection = EHitDirection::Back;
		return hitDirection;
	}
	float rightDot = UKismetMathLibrary::Dot_VectorVector(actorRight, toHit);
	hitDirection = (rightDot > 0.0f) ? EHitDirection::Right : EHitDirection::Left;
	return hitDirection;
}

FRotator ACharacterBase::GetDesiredRotation() const
{
	FVector InputVector = GetLastMovementInputVector();

	if (!InputVector.IsNearlyZero(0.001f))
	{
		// Return rotation based on input vector
		return InputVector.Rotation();
	}
	else
	{
		// No input → return current actor rotation
		return GetActorRotation();
	}
}

float ACharacterBase::TakeDamage(float damage, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	float actualDamage = Super::TakeDamage(damage, damageEvent, eventInstigator, damageCauser);

	if (!CanReciveDamage()) return 0;

	FVector hitLocation;
	if (damageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(damageEvent);
		hitLocation = PointDamageEvent.HitInfo.ImpactPoint;
	}

	UpdateAndGetHitDirection(hitLocation);

	if (bool bCanDamege = PerformHitReaction(hitLocation, damage))
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
		
		float delayTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 1.2;
		FTimerHandle timerHandleUpdateDamage;
		TWeakObjectPtr<ACharacterBase> weakThis = this;
		GetWorld()->GetTimerManager().SetTimer(timerHandleUpdateDamage,
												[weakThis, bCanDamege, damage, eventInstigator, hitLocation]()
												{
													if (weakThis.IsValid())
													{
														weakThis->ApplyDamage(bCanDamege, damage, eventInstigator, hitLocation);
													}
												}, delayTime, false);
	}
	
	return actualDamage;
}

void ACharacterBase::ApplyDamage(bool bCanDamage, float damage, AController* instigatorController, FVector hitLocation)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	
	APlayerController* pC = UGameplayStatics::GetPlayerController(this, 0);
	if (pC->PlayerCameraManager)
	{
		pC->PlayerCameraManager->StartCameraShake( UAttackCameraShake::StaticClass(), 0.5f, ECameraShakePlaySpace::CameraLocal,
												FRotator::ZeroRotator);
	}

	if (!bCanDamage) return;

	APawn* instigatorPawn = instigatorController->GetPawn();
	UAISense_Damage::ReportDamageEvent(GetWorld(), this, instigatorPawn, damage,hitLocation, hitLocation, TEXT("None"));
	
	if (!statsComp->ApplyHealthChange(damage*-1))
	{
		stateManagerComp->SetCurrentState(deadStateTag);
	}
}

void ACharacterBase::OnHealthChanged(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.0f && Delta <= 0.0f)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		DisableInput(PlayerController);
	}
}

void ACharacterBase::OnCharacterStateBegin(FGameplayTag characterState)
{
	if (characterState.MatchesTagExact(deadStateTag))
	{
		FPerformDeath performDeath = PerformDeath();
		
		float delayTime = performDeath.duration + 4.0f;
		TWeakObjectPtr<ACharacterBase> weakThis = this;
		FTimerHandle timerHandleDestroyDeadActors;
		FTimerDelegate timerDelegateDestroyDeadActors;

		//Destroy Attached actors & Self
		timerDelegateDestroyDeadActors.BindLambda([weakThis, performDeath]()
		{
			for (TObjectPtr<AActor> actor : performDeath.actorsToDestory)
			{
				if (actor)
				{
					actor->Destroy();
				}
			}
			if (weakThis.IsValid())
			{
				weakThis->Destroy();
			}
		});
		
		if (timerHandleDestroyDeadActors.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(timerHandleDestroyDeadActors);
		}
		GetWorld()->GetTimerManager().SetTimer(timerHandleDestroyDeadActors, timerDelegateDestroyDeadActors, delayTime, false);
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
	if (combatComp->bIsAttackSaved)
	{
		combatComp->bCanContinueAttack = false;
		
		if (stateManagerComp->GetCurrentState() != attackingStateTag)
		{
			Attack();
			return;
		}
		else
		{
			stateManagerComp->ResetState();
			Attack();
		}
	}
	else
	{
		combatComp->bCanContinueAttack = true;
	}
}

void ACharacterBase::ResetCombat()
{
	combatComp->ResetCombat();
	stateManagerComp->ResetState();
	stateManagerComp->ResetAction();
}

bool ACharacterBase::CanReciveDamage()
{
	FGameplayTagContainer statesToCheck;
	statesToCheck.AddTag(deadStateTag);
	statesToCheck.AddTag(dodgingStateTag);
	return !stateManagerComp->IsCurrentStateEqualToAny(statesToCheck);
}

FPerformDeath ACharacterBase::PerformDeath()
{
	FPerformDeath returnPerformDeath
	{
		{},
		0.0f
	};
	if (!combatComp->GetMainWeapon()) return returnPerformDeath;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	stateManagerComp->SetCurrentAction(dieActionTag);

	TArray<ABaseEquippable*> currentEquipments = equipComp->GetCurrentEquipments();
	for (ABaseEquippable* currentEquipment : currentEquipments)
	{
		returnPerformDeath.actorsToDestory.Add(currentEquipment);
	}

	combatComp->GetMainWeapon()->SimulateWeaponPhysics();
	returnPerformDeath.actorsToDestory.Add(combatComp->GetMainWeapon());
	TArray<UAnimMontage*> actionMontageArray = combatComp->GetMainWeapon()->GetActioMontages(dieActionTag);
	if (!actionMontageArray.IsEmpty()) EnableRagdoll();

	int32 randomIndex = FMath::RandRange(0, actionMontageArray.Num() - 1);
	float animDuration = GetMesh()->GetAnimInstance()->Montage_Play(actionMontageArray[randomIndex]);
	returnPerformDeath.duration = animDuration;

	AAIController* refAIController = Cast<AAIController>(GetController());
	if (!refAIController) return returnPerformDeath;

	refAIController->BrainComponent->StopLogic("Dead");
	return returnPerformDeath;
}

bool ACharacterBase::PerformHitReaction(FVector hitLocation, float damage)
{
	stateManagerComp->SetCurrentAction(disabledStateTag);
	if (ABaseWeapon* mainWeaponRef = combatComp->GetMainWeapon())
	{
		UAnimMontage* hitAnimMontage = nullptr;
		switch (hitDirection)
		{
		case EHitDirection::Front:
			hitAnimMontage = mainWeaponRef->hitMontage_F;
			break;
		case EHitDirection::Back:
			hitAnimMontage = mainWeaponRef->hitMontage_B;
			break;
		case EHitDirection::Left:
			hitAnimMontage = mainWeaponRef->hitMontage_L;
			break;
		case EHitDirection::Right:
			hitAnimMontage = mainWeaponRef->hitMontage_R;
			break;
		default:
			break;
		}
		if (hitAnimMontage)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(hitAnimMontage, 1.0f,
														EMontagePlayReturnType::Duration,
														0.0f,
														true);
			return true;
		}
	}
	return false;
}

FPerformAction ACharacterBase::PerformAction(FGameplayTag characterState, FGameplayTag characterAction,
												int32 montageIndex, bool bRandomIndex)
{
	FPerformAction returnPerformAction
	{
		false,
		0.0f
	};
	
	if (!combatComp->GetMainWeapon()) return returnPerformAction;
	
	TArray<UAnimMontage*> actionMontageArray = combatComp->GetMainWeapon()->GetActioMontages(characterAction);
	if (actionMontageArray.Num() > 0) return returnPerformAction;
	
	int32 randomIndex = FMath::RandRange(0, actionMontageArray.Num() - 1);
	int32 index = bRandomIndex ? randomIndex : montageIndex;
	UAnimMontage* actionMontage = actionMontageArray.IsValidIndex(index) ? actionMontageArray[index] : nullptr;
	if (!actionMontage) return returnPerformAction;

	stateManagerComp->SetCurrentState(characterState);
	stateManagerComp->SetCurrentAction(characterAction);
	float actionDuration = GetMesh()->GetAnimInstance()->Montage_Play(actionMontage, 1.0f,
																	EMontagePlayReturnType::Duration,
																	0.0f,
																	true);
	returnPerformAction.actionDuration = actionDuration;
	returnPerformAction.bSuccess = true;
	return returnPerformAction;
}

FPerformAttack ACharacterBase::PerformAttack(FGameplayTag attackType, int32 attackIndex, bool bRandomIndex,
												bool bIsCalledByAI, float playRate)
{
	FPerformAttack returnPerformAttack
	{
		false,
		0.0f
	};
	
	if (!combatComp->GetMainWeapon()) return returnPerformAttack;
	
	TArray<UAnimMontage*> attackMontageArray = combatComp->GetMainWeapon()->GetActioMontages(attackType);
	if (attackMontageArray.Num() > 0) return returnPerformAttack;
	
	int32 randomIndex = FMath::RandRange(0, attackMontageArray.Num() - 1);
	int32 index = bRandomIndex ? randomIndex : attackIndex;
	TObjectPtr<UAnimMontage> attackMontage = attackMontageArray.IsValidIndex(index) ? attackMontageArray[index] : nullptr;
	if (!attackMontage) return returnPerformAttack;

	stateManagerComp->SetCurrentState(attackingStateTag);
	stateManagerComp->SetCurrentAction(attackType);

	FHitResult hitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetActorLocation(), GetActorLocation(),
													40.0f,objectTypes, false, {},
													EDrawDebugTrace::None, hitResult, true);

	FTransform transform;
	
	FVector targetLocation = (hitResult.bBlockingHit) ?
		(UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(hitResult.GetActor()->GetActorLocation(), GetActorLocation()))
		* 200 + hitResult.GetActor()->GetActorLocation()) : GetActorLocation();
	transform.SetLocation(targetLocation);
	
	FRotator targeRotation = (hitResult.bBlockingHit) ?
		UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), hitResult.GetActor()->GetActorLocation()) : GetActorRotation();
	transform.SetRotation(targeRotation.Quaternion());

	float attackDuration = GetMesh()->GetAnimInstance()->Montage_Play(attackMontage, 1.0f,
																	EMontagePlayReturnType::Duration,
																	0.0f,
																	true);

	combatComp->attackCount++;
	int32 currentAttackIndex = combatComp->attackCount;
	int32 lastMontageIndex = attackMontageArray.Num() - 1;
	if (currentAttackIndex > lastMontageIndex) combatComp->attackCount = 0;

	returnPerformAttack.attackDuration = attackDuration;
	returnPerformAttack.bSuccess = true;
	return returnPerformAttack;
}
#pragma endregion ICombatInterface






