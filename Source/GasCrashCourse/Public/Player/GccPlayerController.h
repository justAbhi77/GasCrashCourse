// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GccPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FGameplayTag;

/**
 * Player controller handling input mapping and ability activation.
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API AGccPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Bind input mappings and actions
	virtual void SetupInputComponent() override;

private:
	// Input mapping contexts to add at the begining
	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Abilities")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Abilities")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Input|Abilities")
	TObjectPtr<UInputAction> TertiaryAction;

	// Jump input handler
	void Jump();

	// Stop jumping handler
	void StopJumping();

	// Movement handler
	void Move(const FInputActionValue& Value);

	// Mouse look handler
	void Look(const FInputActionValue& Value);

	// Primary (ability) input
	void Primary();

	// Activate ability with tag
	void ActivateAbility(const FGameplayTag& AbilityTag) const;

	// Secondary ability input
	void Secondary();

	// Tertiary ability input
	void Tertiary();

	// If true, player can mouse-look while dead
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|Input|Abilities", meta = (AllowPrivateAccess = "true"))
	bool bCanMouseLookIfDead = false;

	bool IsAlive() const;
};
