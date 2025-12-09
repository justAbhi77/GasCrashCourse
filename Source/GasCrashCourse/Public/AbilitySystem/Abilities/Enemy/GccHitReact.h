// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GccGameplayAbility.h"
#include "GccHitReact.generated.h"

/**
 * Hit React Ability for Npcs
 * Uses the direction vectors to determine which hit react montage section to play
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API UGccHitReact : public UGccGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * Caches the direction vectors needed for hit react montage section selection
	 * @param Instigator The actor that caused the hit react
	 * @param OutAvatarForward Avatar's forward vector
	 * @param OutToInstigator Normalized vector pointing from avatar to instigator
	 */
	UFUNCTION(BlueprintCallable, Category = "Gas Crash|Abilities")
	void CacheHitDirectionVectors(AActor* Instigator, FVector& OutAvatarForward, FVector& OutToInstigator);

	UPROPERTY(BlueprintReadOnly, Category = "Gas Crash|Abilities")
	FVector AvatarForward;

	UPROPERTY(BlueprintReadOnly, Category = "Gas Crash|Abilities")
	FVector ToInstigator;
};
