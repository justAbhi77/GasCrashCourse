// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "GccWaitGameplayEvent.generated.h"

/**
 * Nothing fancy, just a simple wrapper for UAbilityAsync_WaitGameplayEvent
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API UGccWaitGameplayEvent : public UAbilityAsync_WaitGameplayEvent
{
	GENERATED_BODY()
public:
	/**
	 * Async node generator to wait for a gameplay event with a specific tag to be triggered on the actor.
	 * @param TargetActor The actor to listen for gameplay events on.
	 * @param EventTag The gameplay event tag to listen for. Must be under Events Category.
	 * @param OnlyTriggerOnce Whether to only trigger once or keep listening for events.
	 * @param OnlyMatchExact Whether to only match the exact tag or also match parent tags.
	 * @return The ability task created.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UGccWaitGameplayEvent* WaitGameplayEventToActorProxy(AActor* TargetActor, UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void StartActivation();
};
