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
 *
 */
UCLASS()
class GASCRASHCOURSE_API AGccPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Abilities")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Abilities")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Input|Abilities")
	TObjectPtr<UInputAction> TertiaryAction;

	void Jump();

	void StopJumping();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Primary();

	void ActivateAbility(const FGameplayTag& AbilityTag) const;

	void Secondary();

	void Tertiary();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|Input|Abilities", meta = (AllowPrivateAccess = "true"))
	bool bCanMouseLookIfDead = false;

	bool IsAlive() const;
};
