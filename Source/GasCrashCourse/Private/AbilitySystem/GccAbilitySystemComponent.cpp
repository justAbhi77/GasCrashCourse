// Copyright. All Rights Reserved.


#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "GameplayTags/GccTags.h"


UGccAbilitySystemComponent::UGccAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGccAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGccAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UGccAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

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
		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UGccAbilitySystemComponent::AddToAbilityLevel(const TSubclassOf<UGameplayAbility> AbilityClass, const int32 LevelsToAdd)
{
	if(IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level += LevelsToAdd;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UGccAbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if(!IsValid(AbilitySpec.Ability)) return;

	for(const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
		if(Tag.MatchesTagExact(GccTags::GccAbilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);
			// we can not use GiveAbilityAndActivateOnce here because it would remove the ability from the ability system component
			return;
		}
}

