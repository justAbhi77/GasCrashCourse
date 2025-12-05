// Copyright. All Rights Reserved.


#include "AbilitySystem/GccAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/GccTags.h"

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

	if(Data.EvaluatedData.Attribute == GetHealthAttribute() && GetHealth() <= 0.f)
	{
		FGameplayEventData Payload;
		Payload.Instigator = Data.Target.GetAvatarActor();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.EffectSpec.GetEffectContext().GetInstigator(), GccTags::Events::KillScored, Payload);
	}

	if(Data.EvaluatedData.Attribute == GetManaAttribute() && bAttributesInitialized)
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));

	if(Data.EvaluatedData.Attribute == GetHealthAttribute() && bAttributesInitialized)
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
	OnAttributesInitialized.Broadcast();
}
