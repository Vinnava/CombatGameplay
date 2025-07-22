// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Character/Base/CharacterBase.h"
#include "PlayerBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAY_API APlayerBase : public ACharacterBase
{
	GENERATED_BODY()

private://Variables
#pragma region InputVariables
	/** MappingContext */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|MappingContexts", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* defaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|MappingContexts", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* menuMappingContext;

	/** Move Input Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	class UInputAction* moveAction;

	/** Look Input Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* lookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* lightAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* dodge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleWalk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleTutorial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialize|Input|InputActions", meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleMenu;
	
#pragma endregion InputVariables

	UPROPERTY(EditDefaultsOnly, Category = "Initizalize|MotionWarping")
	FName dodgeWarpTargetName;
	
	bool bIsMenuUp;
	
protected://Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> followCamera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> cameraBoom;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> cameraBobbler;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCharacterTrajectoryComponent> characterTrajectoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector2D movementValue;

public://Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = WidgetRef)
	TObjectPtr<class UPlayerWidget> playerWidgetRef;

private://Functions
	void BobbleCamera() const;

protected://Functions
#pragma region InputComponents
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const struct  FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void LightAttack(const FInputActionValue& value);
	void Dodge(const FInputActionValue& value);
	void ToggleWalk(const FInputActionValue& value);
	void ToggleTutorial(const FInputActionValue& value);
	void ToggleMenu(const FInputActionValue& value);
#pragma endregion InputComponents

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void EnableRagdoll() const override;
	
public://Functions
	APlayerBase();
	
	virtual FPerformDeath PerformDeath() override;
	virtual void OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta) override;
};

