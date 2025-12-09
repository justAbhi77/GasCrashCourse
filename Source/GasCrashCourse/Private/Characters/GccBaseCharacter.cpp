// Copyright. All Rights Reserved.

#include "Characters/GccBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Utils/DebugUtil.h"

AGccBaseCharacter::AGccBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Make hitboxes work on server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AGccBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bAlive);
}

UAbilitySystemComponent* AGccBaseCharacter::GetAbilitySystemComponent() const
{
	// Override in child classes
	return nullptr;
}

void AGccBaseCharacter::GiveStartupAbilities()
{
	if(!IsValid(GetAbilitySystemComponent()))
	{
		PRINT_DEBUG_WARNING("ASC invalid. Cannot give startup abilities");
		return;
	}

	for(const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}

	PRINT_DEBUG("Startup abilities granted");
}

void AGccBaseCharacter::InitializeAttributes() const
{
	if(!IsValid(InitializeAttributesEffect))
	{
		PRINT_DEBUG_WARNING("InitializeAttributesEffect missing");
		return;
	}

	if(!IsValid(GetAbilitySystemComponent()))
	{
		PRINT_DEBUG_WARNING("ASC missing during InitializeAttributes");
		return;
	}

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	PRINT_DEBUG("Initial attributes applied");

	if(UGccAttributeSet* AttributeSet = Cast<UGccAttributeSet>(GetAttributeSet()))
		AttributeSet->PostAttributeInitialized();
}

void AGccBaseCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	PRINT_DEBUG("Health changed %f", AttributeChangeData.NewValue);

	if(AttributeChangeData.NewValue <= 0.f)
		HandleDeath();
}

void AGccBaseCharacter::HandleDeath()
{
	bAlive = false;
	PRINT_DEBUG("%s has died", *GetName());
}

void AGccBaseCharacter::HandleRespawn()
{
	bAlive = true;
	PRINT_DEBUG("Character respawned");
}

void AGccBaseCharacter::ResetAttributes() const
{
	if(!IsValid(ResetAttributesEffect))
	{
		PRINT_DEBUG_WARNING("ResetAttributesEffect missing");
		return;
	}

	if(!IsValid(GetAbilitySystemComponent()))
	{
		PRINT_DEBUG_WARNING("ASC missing during ResetAttributes");
		return;
	}

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ResetAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	PRINT_DEBUG("Attributes reset");
}
