// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "GccWaitGameplayEvent.generated.h"

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccWaitGameplayEvent : public UAbilityAsync_WaitGameplayEvent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UGccWaitGameplayEvent* WaitGameplayEventToActorProxy(AActor* TargetActor, UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void StartActivation();
};
