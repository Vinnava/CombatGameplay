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
	cameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, -31.0f));
	cameraBoom->SocketOffset = FVector(0.0f, 50.0f, 0.0f);
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCamera->bUsePawnControlRotation = false; // The Camera does not rotate relative to the arm

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
	if (!combatComp) UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BeginPlay] CombatComponent is missing!"), *GetClass()->GetName());

	if (!stateManagerComp) UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BeginPlay] StateManagerComponent is missing!"), *GetClass()->GetName());
	
	if (!playerWidgetRef || !playerWidgetRef->GetHealthBarWidget()) UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [BeginPlay] PlayerWidget or HealthBarWidget is not assigned!"), *GetClass()->GetName());
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
				UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Default input mapping context added."), *GetClass()->GetName());
			}
			else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetupPlayerInputComponent] Failed to get EnhancedInputLocalPlayerSubsystem."), *GetClass()->GetName());
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind input actions
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		EnhancedInputComponent->BindAction(lightAttack, ETriggerEvent::Started, this, &APlayerBase::LightAttack);
		EnhancedInputComponent->BindAction(dodge, ETriggerEvent::Started, this, &APlayerBase::Dodge);
		EnhancedInputComponent->BindAction(toggleWalk, ETriggerEvent::Started, this, &APlayerBase::ToggleWalk);
		EnhancedInputComponent->BindAction(toggleTutorial, ETriggerEvent::Started, this, &APlayerBase::ToggleTutorial);
		EnhancedInputComponent->BindAction(toggleMenu, ETriggerEvent::Started, this, &APlayerBase::ToggleMenu);
		
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Input actions bound successfully."), *GetClass()->GetName());
	}
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetupPlayerInputComponent] Failed to cast PlayerInputComponent to UEnhancedInputComponent."), *GetClass()->GetName());
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
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [Move] Cannot Move, Controller reference is Null."), *GetClass()->GetName());
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
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [Look] Cannot Look, Controller reference is Null."), *GetClass()->GetName());
}

void APlayerBase::LightAttack(const FInputActionValue& value)
{
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] LightAttack triggered."), *GetClass()->GetName());

	if (!combatComp || !stateManagerComp)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [LightAttack] Cannot perform LightAttack: combatComp or stateManagerComp is null"), *GetClass()->GetName());
		return;
	}

	if (stateManagerComp->GetCurrentState() != GameplayTags::State::Attacking())
	{
		// Start Combo Attack
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Start Combo Attack."), *GetClass()->GetName());
		Attack();
		return;
	}

	if (combatComp->bCanContinueAttack)
	{
		// Continue Attack
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Continuing Attack & ResetAttack"), *GetClass()->GetName());
		combatComp->bCanContinueAttack = false;
		stateManagerComp->ResetState();
		
		Attack();
	}
	else
	{
		// Attack saved to be continued
		combatComp->bIsAttackSaved = true;
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Attack saved to be continued."), *GetClass()->GetName());
	}
}

void APlayerBase::Dodge(const FInputActionValue& value)
{
	if (!CanPerformDodge())
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [CanPerformDodge] Cannot Perform Dodge, CanPerformDodge returns False"), *GetClass()->GetName());
		return;
	}
	//MotionWarp & Dodge
	motionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(dodgeWarpTargetName, GetActorLocation(), GetDesiredRotation());
	PerformAction(GameplayTags::State::Dodging(), GameplayTags::Action::Dodge(), 0, false);
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] Performing Dodge."), *GetClass()->GetName());
}

void APlayerBase::ToggleWalk(const FInputActionValue& value)
{
	if (GetMovementSpeedMode() == EMovementSpeedMode::Walking)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Jogging);
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] EMovementSpeedMode::Jogging."), *GetClass()->GetName());
	}
	else if (GetMovementSpeedMode() == EMovementSpeedMode::Jogging)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Walking);
		UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] EMovementSpeedMode::Walking."), *GetClass()->GetName());
	}
}

void APlayerBase::ToggleTutorial(const FInputActionValue& value)
{
	if (!playerWidgetRef)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleTutorial] playerWidgetRef returns Null"), *GetClass()->GetName());
		return;
	}
	playerWidgetRef->ToggleTutorial();
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] ToggleTutorial"), *GetClass()->GetName());
}

void APlayerBase::ToggleMenu(const FInputActionValue& value)
{
	if (!playerWidgetRef)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleTutorial] playerWidgetRef returns Null"), *GetClass()->GetName());
		return;
	}
	playerWidgetRef->ToggleMenu();
	UE_LOG(GPLogPlayerBase, Log, TEXT("[%s] ToggleMenu"), *GetClass()->GetName());
}

#pragma endregion InputComponents

void APlayerBase::ChangeInputMappingContext(const UInputMappingContext* newMappingContext)
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ChangeInputMappingContext] Failed: PlayerController is null."), *GetClass()->GetName());
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
	{
		subsystem->RemoveMappingContext(currentMappingContext);
		subsystem->AddMappingContext(newMappingContext, 0);
		currentMappingContext = newMappingContext;
	}
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ChangeInputMappingContext] Failed: MappingContext."), *GetClass()->GetName());
}

void APlayerBase::SetDefaultInputMappingContext()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetDefaultInputMappingContext] Failed: PlayerController is null."), *GetClass()->GetName());
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
	{
		subsystem->RemoveMappingContext(currentMappingContext);
		subsystem->AddMappingContext(defaultMappingContext, 0);
		currentMappingContext = defaultMappingContext;
	}
	else UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [SetDefaultInputMappingContext] Failed: MappingContext."), *GetClass()->GetName());
}

void APlayerBase::BobbleCamera() const
{
	if (!cameraBoom || !cameraBobbler)
	{
		UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [BobbleCamera] Failed: cameraBoom or cameraBobbler is null."), *GetClass()->GetName());
		return;
	}

	// Smoothly interpolates camera boom location to reduce sharp head motion
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
	else UE_LOG(GPLogPlayerBase, Error, TEXT("[%s] [EnableRagdoll] Failed: CameraBoom is null."), *GetClass()->GetName());
	
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"), true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, true, true);
}

FPerformDeath APlayerBase::PerformDeath()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	
	if (!playerController || !playerWidgetRef)
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [ToggleMenu] Failed: PlayerController or PlayerWidgetRef is null."), *GetClass()->GetName());
		return {};
	}
	
	FInputModeUIOnly inputModeData;
	inputModeData.SetWidgetToFocus(playerWidgetRef->TakeWidget());
	inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputModeData);
	
	playerController->SetShowMouseCursor(true);
	playerWidgetRef->ToggleMenu();
	
	return Super::PerformDeath();
}

void APlayerBase::OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta)
{
	if (!playerWidgetRef || !playerWidgetRef->GetHealthBarWidget())
	{
		UE_LOG(GPLogPlayerBase, Warning, TEXT("[%s] [OnHealthChanged] Cannot update health UI: Widget or health widget is null."), *GetClass()->GetName());
		return;
	}
	
	playerWidgetRef->GetHealthBarWidget()->UpdateHealth(newHealth);
}
