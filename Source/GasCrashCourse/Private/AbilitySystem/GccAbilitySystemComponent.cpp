// Copyright. All Rights Reserved.

#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "GameplayTags/GccTags.h"
#include "Utils/DebugUtil.h"

void UGccAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	PRINT_DEBUG("Ability %s given to %s at level: %d", *AbilitySpec.Ability->GetName(), *GetOwnerActor()->GetName(), AbilitySpec.Level);
	HandleAutoActivatedAbility(AbilitySpec);
}

void UGccAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// this is a mutex lock to prevent ability list changes while we are iterating through it
	// we can use either of the two locking mechanisms here
	// FScopedAbilityListLock ActiveScopeLock(*this); or ABILITYLIST_SCOPE_LOCK()
	// either one works the same way
	ABILITYLIST_SCOPE_LOCK()
	for(const auto& AbilitySpec : GetActivatableAbilities())
		HandleAutoActivatedAbility(AbilitySpec);
}

void UGccAbilitySystemComponent::SetAbilityLevel(const TSubclassOf<UGameplayAbility> AbilityClass, const int32 Level)
{
	if(IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		PRINT_DEBUG("%s -> Level %d", *AbilityClass->GetName(), Level);

		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
	else
		PRINT_DEBUG_WARNING("failed: Ability %s not found", *AbilityClass->GetName());
}

void UGccAbilitySystemComponent::AddToAbilityLevel(const TSubclassOf<UGameplayAbility> AbilityClass, const int32 LevelsToAdd)
{
	if(IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		PRINT_DEBUG("%s -> +%d Levels", *AbilityClass->GetName(), LevelsToAdd);

		AbilitySpec->Level += LevelsToAdd;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
	else
		PRINT_DEBUG_WARNING("failed: Ability %s not found", *AbilityClass->GetName());
}

void UGccAbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if(!IsValid(AbilitySpec.Ability))
	{
		PRINT_DEBUG_WARNING("Invalid AbilitySpec.Ability");
		return;
	}

	for(const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
		if(Tag.MatchesTagExact(GccTags::GccAbilities::ActivateOnGiven))
		{
			PRINT_DEBUG("Activating %s", *AbilitySpec.Ability->GetName());

			// we can not use GiveAbilityAndActivateOnce here because it would remove the ability from the ability system component
			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
}
