// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GccGameplayAbility.generated.h"

/**
 * Base Gameplay Ability class for GAS Crash Course.
 * Adds optional debug visualization and unified logging for ability lifecycle.
 */
UCLASS()
class GASCRASHCOURSE_API UGccGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Enables debug for the ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GasCrash|Debug")
	bool bDrawDebugs = false;

	// Called when the ability is activated
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Called when the ability ends (normally or via cancellation)
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
