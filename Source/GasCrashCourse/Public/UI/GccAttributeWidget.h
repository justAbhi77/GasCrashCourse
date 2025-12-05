// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "GccAttributeWidget.generated.h"

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccAttributeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|Attributes")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|Attributes")
	FGameplayAttribute MaxAttribute;

	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, const UGccAttributeSet* AttributeSet, float OldValue);
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attribute Change"))
	void BP_OnAttributeChange(float NewValue, float NewMaxValue, float OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "GasCrash|Attributes")
	TWeakObjectPtr<AActor> AvatarActor;
};
