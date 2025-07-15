// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterTrajectoryComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Components/CombatComponent.h"
#include "Gameplay/Components/StateManagerComponent.h"
#include "Gameplay/Data/GameplayData.h"
#include "Gameplay/UI/Player/HealthBarWidget.h"
#include "Gameplay/UI/Player/PlayerWidget.h"
#include "Gameplay/Data/GameplayTagData.h"


APlayerBase::APlayerBase()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Creating Camera Bobbler
	cameraBobbler = CreateDefaultSubobject<USceneComponent>(TEXT("CameraBobbler"));
	cameraBobbler->SetupAttachment(GetMesh(), TEXT("head"));

	//Creating MotionTrajectory Component
	characterTrajectoryComp = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("characterTrajectoryComponent"));
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	BobbleCamera();
}

#pragma region InputComponents

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);

		// Looking
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);

		// LightAttack
		EnhancedInputComponent->BindAction(lightAttack, ETriggerEvent::Started, this, &APlayerBase::LightAttack);

		// Dodge
		EnhancedInputComponent->BindAction(dodge, ETriggerEvent::Started, this, &APlayerBase::Dodge);
	}
	else
	{
		UE_LOG(LogBaseCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	movementValue = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, movementValue.Y);
		AddMovementInput(RightDirection, movementValue.X);
	}
}

void APlayerBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerBase::LightAttack(const FInputActionValue& value)
{
	if (stateManagerComp->GetCurrentState() != attackingStateTag)
	{
		Attack();
		return;
	}
	if (!combatComp->bCanContinueAttack)
	{
		combatComp->bIsAttackSaved = true;
		return;
	}
	combatComp->bIsAttackSaved = false;
	stateManagerComp->ResetState();
	Attack();
}

void APlayerBase::Dodge(const FInputActionValue& value)
{
	if (!CanPerformDodge()) return;

	motionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(dodgeWarpTargetName, GetActorLocation(), GetDesiredRotation());
	PerformAction(dodgingStateTag, dodgeActionTag, 0, false);
}

void APlayerBase::ToggleWalk(const FInputActionValue& value)
{
	if (GetMovementSpeedMode() == EMovementSpeedMode::Walking)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Jogging);
	}
	else if (GetMovementSpeedMode() == EMovementSpeedMode::Jogging)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Walking);
	}
}

void APlayerBase::ToggleToturial(const FInputActionValue& value)
{
	playerWidgetRef->ToggleTutorial();
}

void APlayerBase::ToggleMenu(const FInputActionValue& value)
{
	if (bIsMenuUp == true)
	{
		bIsMenuUp = false;
		
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		
		playerController->SetShowMouseCursor(false);
		playerWidgetRef->RemoveRestartWidget();
		
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->RemoveMappingContext(menuMappingContext);
			subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}
	else
	{
		bIsMenuUp = true;

		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		FInputModeGameAndUI inputMode;
		inputMode.SetWidgetToFocus(playerWidgetRef->TakeWidget());
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		inputMode.SetHideCursorDuringCapture(true);
		playerController->SetInputMode(inputMode);

		playerController->SetShowMouseCursor(true);
		playerWidgetRef->RestatWidget();

		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->RemoveMappingContext(defaultMappingContext);
			subsystem->AddMappingContext(menuMappingContext, 0);
		}
	}
}

#pragma endregion InputComponents

void APlayerBase::BobbleCamera()
{
	FVector start = cameraBoom->GetComponentLocation();
	FVector end = cameraBobbler->GetComponentLocation();
	FVector halfway = FMath::Lerp(start, end, 0.05f);
	cameraBoom->SetWorldLocation(halfway, false, nullptr);
}

void APlayerBase::EnableRagdoll() const
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None, 0);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	cameraBoom->AttachToComponent(GetMesh(),  FAttachmentTransformRules::KeepWorldTransform, pelvisBoneName);
	cameraBoom->bDoCollisionTest = false;
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"), true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, true, true);
}

FPerformDeath APlayerBase::PerformDeath()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	FInputModeUIOnly inputModeData;
	inputModeData.SetWidgetToFocus(playerWidgetRef->TakeWidget());
	inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputModeData);
	
	playerController->SetShowMouseCursor(true);
	playerWidgetRef->RestatWidget();
	
	return Super::PerformDeath();
}

void APlayerBase::OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta)
{
	playerWidgetRef->healthWidget->UpdateHealth(newHealth);
}
