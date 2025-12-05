// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GccAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASCRASHCOURSE_API UGccAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGccAbilitySystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 LevelsToAdd = 1);
protected:
	virtual void BeginPlay() override;

private:
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
};
