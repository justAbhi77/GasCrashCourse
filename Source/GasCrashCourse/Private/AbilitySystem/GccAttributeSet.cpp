// Copyright. All Rights Reserved.

#include "AbilitySystem/GccAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/GccTags.h"
#include "Utils/DebugUtil.h"

void UGccAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	REGISTER_REPLICATED_ATTRIBUTE(Health);
	REGISTER_REPLICATED_ATTRIBUTE(MaxHealth);
	REGISTER_REPLICATED_ATTRIBUTE(Mana);
	REGISTER_REPLICATED_ATTRIBUTE(MaxMana);

	DOREPLIFETIME(ThisClass, bAttributesInitialized);
}

void UGccAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	// Health reaching zero
	if(Attribute == GetHealthAttribute() && GetHealth() <= 0.f)
	{
		PRINT_DEBUG("Health reached zero — broadcasting KillScored event");

		FGameplayEventData Payload;
		Payload.Instigator = Data.Target.GetAvatarActor();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.EffectSpec.GetEffectContext().GetInstigator(), GccTags::Events::KillScored, Payload);
	}

	// Clamp Mana
	if(Attribute == GetManaAttribute() && bAttributesInitialized)
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));

	// Clamp Health
	if(Attribute == GetHealthAttribute() && bAttributesInitialized)
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
}

void UGccAttributeSet::OnRep_AttributesInitialized()
{
	if(bAttributesInitialized)
		OnAttributesInitialized.Broadcast();
}

void UGccAttributeSet::PostAttributeInitialized()
{
	if(bAttributesInitialized) return;

	bAttributesInitialized = true;

	PRINT_DEBUG("broadcasting to listeners");

	OnAttributesInitialized.Broadcast();
}
