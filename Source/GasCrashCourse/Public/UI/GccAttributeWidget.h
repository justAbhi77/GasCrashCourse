// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "GccAttributeWidget.generated.h"

/**
 * Base Widget class for displaying Attribute information
 * Exposes Attribute and MaxAttribute properties to specify which Attribute to observe
 * Exposes OnAttributeChange event to respond to changes in the observed Attributes
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API UGccAttributeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|Attributes")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|Attributes")
	FGameplayAttribute MaxAttribute;

	void OnAttributeChange(const UGccAttributeSet* AttributeSet, float OldValue);
	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, const UGccAttributeSet* AttributeSet, float OldValue);
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Gas Crash", meta = (DisplayName = "On Attribute Change"))
	void BP_OnAttributeChange(float NewValue, float NewMaxValue, float OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "Gas Crash|Attributes")
	TWeakObjectPtr<AActor> AvatarActor;
};
