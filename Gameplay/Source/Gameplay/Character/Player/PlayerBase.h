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
	class UInputAction* lookAction;
#pragma endregion InputVariables
	
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


protected://Functions
#pragma region Input
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const struct  FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
#pragma endregion Input

	virtual void BeginPlay() override;
	
	virtual void EnableRagdoll() const override;
	virtual FPerformDeath PerformDeath() override;
	virtual void OnHealthChanged(AActor* instigatorActor, UStatsComponent* owningComp, float newHealth, float delta) override;
	
public://Functions
	APlayerBase();
};

