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
#include "Gameplay/Data/GameplayTagLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(GPLogPlayerBase, Log, All);

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

	// Ensure combat and state manager components are initialized
	if (!combatComp) UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BeginPlay] CombatComponent is missing!"), *GetName());

	if (!stateManagerComp) UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BeginPlay] StateManagerComponent is missing!"), *GetName());
	
	if (!playerWidgetRef || !playerWidgetRef->healthWidget) UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [BeginPlay] PlayerWidget or HealthBarWidget is not assigned!"), *GetName());
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	BobbleCamera();
}

#pragma region InputComponents

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Setup Enhanced Input Mapping
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(defaultMappingContext, 0);
				UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Default input mapping context added."), *GetName());
			}
			else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetupPlayerInputComponent] Failed to get EnhancedInputLocalPlayerSubsystem."), *GetName());
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind input actions
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		EnhancedInputComponent->BindAction(lightAttack, ETriggerEvent::Started, this, &APlayerBase::LightAttack);
		EnhancedInputComponent->BindAction(toggleWalk, ETriggerEvent::Started, this, &APlayerBase::ToggleWalk);
		EnhancedInputComponent->BindAction(toggleTutorial, ETriggerEvent::Started, this, &APlayerBase::ToggleTutorial);
		EnhancedInputComponent->BindAction(toggleMenu, ETriggerEvent::Started, this, &APlayerBase::ToggleMenu);
		
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Input actions bound successfully."), *GetName());
	}
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetupPlayerInputComponent] Failed to cast PlayerInputComponent to UEnhancedInputComponent."), *GetName());
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
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [Move] Cannot Move, Controller reference is Null."), *GetName());
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
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [Look] Cannot Look, Controller reference is Null."), *GetName());
}

void APlayerBase::LightAttack(const FInputActionValue& value)
{
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] LightAttack triggered."), *GetName());

	if (!combatComp || !stateManagerComp)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [LightAttack] Cannot perform LightAttack: combatComp or stateManagerComp is null"), *GetName());
		return;
	}

	if (stateManagerComp->GetCurrentState() != GameplayTags::State::Attacking())
	{
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Continuing Attack."), *GetName());
		Attack();
		return;
	}

	if (!combatComp->bCanContinueAttack)
	{
		combatComp->bIsAttackSaved = true;
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Attack saved to be continued."), *GetName());
		return;
	}

	combatComp->bIsAttackSaved = false;
	stateManagerComp->ResetState();
	Attack();
}

void APlayerBase::Dodge(const FInputActionValue& value)
{
	if (!CanPerformDodge())
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] Cannot Perform Dodge."), *GetName());
		return;
	}

	motionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(dodgeWarpTargetName, GetActorLocation(), GetDesiredRotation());
	PerformAction(GameplayTags::State::Dodging(), GameplayTags::Action::Dodge(), 0, false);
}

void APlayerBase::ToggleWalk(const FInputActionValue& value)
{
	if (GetMovementSpeedMode() == EMovementSpeedMode::Walking)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Jogging);
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] EMovementSpeedMode::Jogging."), *GetName());
	}
	else if (GetMovementSpeedMode() == EMovementSpeedMode::Jogging)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Walking);
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] EMovementSpeedMode::Walking."), *GetName());
	}
}

void APlayerBase::ToggleTutorial(const FInputActionValue& value)
{
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] ToggleTutorial."), *GetName());
	playerWidgetRef->ToggleTutorial();
}

void APlayerBase::ToggleMenu(const FInputActionValue& value)
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController || !playerWidgetRef)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleMenu] Failed: PlayerController or PlayerWidgetRef is null."), *GetName());
		return;
	}

	// Rest of the function remains, but add logs like:
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Menu %s."), *GetName(), bIsMenuUp ? TEXT("closed") : TEXT("opened"));

	if (bIsMenuUp == true)
	{
		bIsMenuUp = false;
		
		FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		
		playerController->SetShowMouseCursor(false);
		playerWidgetRef->RemoveRestartWidget();
		
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->RemoveMappingContext(menuMappingContext);
			subsystem->AddMappingContext(defaultMappingContext, 0);
		}
		else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleMenu] Failed: MappingContext."), *GetName());
	}
	else
	{
		bIsMenuUp = true;
		
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
		else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleMenu] Failed: MappingContext."), *GetName());
	}
}

#pragma endregion InputComponents

void APlayerBase::BobbleCamera()
{
	// Smoothly interpolates camera boom location to reduce sharp head motion
	if (!cameraBoom || !cameraBobbler)
	{
		UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BobbleCamera] Failed: cameraBoom or cameraBobbler is null."), *GetName());
		return;
	}
	
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
	if (cameraBoom)
	{
		cameraBoom->AttachToComponent(GetMesh(),  FAttachmentTransformRules::KeepWorldTransform, pelvisBoneName);
		cameraBoom->bDoCollisionTest = false;
	}
	else UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [EnableRagdoll] Failed: CameraBoom is null."), *GetName());
	
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"), true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, true, true);
}

FPerformDeath APlayerBase::PerformDeath()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	
	if (!playerController || !playerWidgetRef)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleMenu] Failed: PlayerController or PlayerWidgetRef is null."), *GetName());
		return {};
	}
	
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
	if (!playerWidgetRef || !playerWidgetRef->healthWidget)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [OnHealthChanged] Cannot update health UI: Widget or health widget is null."), *GetName());
		return;
	}
	
	playerWidgetRef->healthWidget->UpdateHealth(newHealth);
}
