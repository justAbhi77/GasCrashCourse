// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GccBaseCharacter.h"
#include "GccEnemyCharacter.generated.h"

class UAttributeSet;

/**
 * This class implements an AI-driven enemy character with ASC, attributes, AI control and launch/land handling.
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API AGccEnemyCharacter : public AGccBaseCharacter
{
	GENERATED_BODY()

public:
	AGccEnemyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UAttributeSet* GetAttributeSet() const override;

	// Radius for AI to consider near
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|AI")
	float AcceptanceRadius{500.f};

	// Minimum delay between enemy attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|AI")
	float MinAttackDelay{.1f};

	// Maximum delay between enemy attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|AI")
	float MaxAttackDelay{.5f};

	// Returns animation timeline length from BP
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
	float GetTimelineLength();

	// Tracks whether the enemy is airborne
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsBeingLaunched{false};

	// Disables movement until after landing
	void StopMovementUntilLanded();

protected:

	virtual void BeginPlay() override;

	virtual void HandleDeath() override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// Restores movement after landing
	UFUNCTION()
	void EnableMovementOnLanded(const FHitResult& Hit);
};
