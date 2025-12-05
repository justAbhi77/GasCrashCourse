// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GccGameplayAbility.h"
#include "GccPrimary.generated.h"

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccPrimary : public UGccGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GasCrash|Abilities")
	// ReSharper disable once CppUEBlueprintCallableFunctionUnused
	void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GasCrash|Abilities")
	float HitBoxRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GasCrash|Abilities")
	float HitBoxForwardOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GasCrash|Abilities")
	float HitBoxElevationOffset = 20.0f;
};
