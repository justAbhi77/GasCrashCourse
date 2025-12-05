// Copyright. All Rights Reserved.


#include "Player/GccPlayerState.h"
#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"

AGccPlayerState::AGccPlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UGccAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGccAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AGccPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}