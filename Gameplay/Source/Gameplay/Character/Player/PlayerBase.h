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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* defaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* moveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* lightAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* dodge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleWalk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleTutorial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* toggleMenu;
	
#pragma endregion InputVariables

	UPROPERTY(EditDefaultsOnly, Category = "Initizalize|MotionWarping")
	FName dodgeWarpTargetName;
	
protected://Variables
	TObjectPtr<class UCameraComponent> followCamera;
	TObjectPtr<class USpringArmComponent> cameraBoom;
	TObjectPtr<USceneComponent> cameraBobbler;
	TObjectPtr<class UCharacterTrajectoryComponent> characterTrajectoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector2D movementValue;

public://Variables
	TObjectPtr<APlayerController> playerController;
	TObjectPtr<class UPlayerWidget> playerWidgetRef;

private://Functions
	void BobbleCamera();

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
#pragma endregion InputComponents

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void EnableRagdoll() const override;
	virtual FPerformDeath PerformDeath() override;
	virtual void OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta) override;
	
public://Functions
	APlayerBase();
};

