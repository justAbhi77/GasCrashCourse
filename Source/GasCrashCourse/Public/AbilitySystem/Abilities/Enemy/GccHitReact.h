// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GccGameplayAbility.h"
#include "GccHitReact.generated.h"

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccHitReact : public UGccGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GasCrash|Abilities")
	void CacheHitDirectionVectors(AActor* Instigator, FVector& OutAvatarForward, FVector& OutToInstigator);

	UPROPERTY(BlueprintReadOnly, Category = "GasCrash|Abilities")
	FVector AvatarForward;

	UPROPERTY(BlueprintReadOnly, Category = "GasCrash|Abilities")
	FVector ToInstigator;
};
