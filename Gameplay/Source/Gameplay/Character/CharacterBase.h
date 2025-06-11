// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interface/GameplayTagInterface.h"
#include "Logging/LogMacros.h"
#include "CharacterBase.generated.h"

class UStateManagerComponent;
class UStatsComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ACharacterBase : public ACharacter, public IGameplayTagInterface
{
	GENERATED_BODY()
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;


public:
	ACharacterBase();

	virtual void PostInitializeComponents() override;

protected:
//Functions
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

private://Variables

protected://Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector2D movementValue;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Components)
	TObjectPtr<UStatsComponent> statsComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Components)
	TObjectPtr<UStateManagerComponent> stateManagerComp ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Initialize)
	FGameplayTag ownedGameplayTag;
	
public://Variables

private://Functions

protected://Functions
	UFUNCTION(BlueprintNativeEvent, Category = Stats)
	void OnHealthChanged(AActor* InstigatorActor, UStatsComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION(BlueprintNativeEvent, Category = StateManager)
	void OnCharacterStateBegin(FGameplayTag characterState);
	
	
public://Functions
#pragma region IGameplayTagInterface
	const FGameplayTag& GetOwnedGameplayTag() const override;
	bool HasMatchingGameplayTag(FGameplayTagContainer tagsToCheck) override;
#pragma endregion IGameplayTagInterface

};

