// Copyright. All Rights Reserved.


#include "GasCrashCourse/Public/Characters/GccBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/GccAttributeSet.h"


AGccBaseCharacter::AGccBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Tick and refresh bone transforms whether rendered or not - for bone updates on a dedicated server
	// To make Hitboxes from animation work correctly in game (generally for a server based animation)
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AGccBaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bAlive);
}

UAbilitySystemComponent* AGccBaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AGccBaseCharacter::GiveStartupAbilities()
{
	if(!IsValid(GetAbilitySystemComponent())) return;

	for(const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}

void AGccBaseCharacter::InitializeAttributes() const
{
	// checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set."));
	if(!IsValid(InitializeAttributesEffect))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Attribute Effects. InitializeAttributesEffect not set.", true, true, FLinearColor(1,0,0,1), 5.f);
		return;
	}

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	if(UGccAttributeSet* AttributeSet = Cast<UGccAttributeSet>(GetAttributeSet()))
		AttributeSet->PostAttributeInitialized();
}

void AGccBaseCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if(AttributeChangeData.NewValue <= 0.f)
		HandleDeath();
}

void AGccBaseCharacter::HandleDeath()
{
	bAlive = false;

	if(IsValid(GEngine))
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s has died!"), *GetName()));
}

void AGccBaseCharacter::HandleRespawn()
{
	bAlive = true;
}

void AGccBaseCharacter::ResetAttributes() const
{
	// checkf(IsValid(ResetAttributesEffect), TEXT("ResetAttributesEffect not set."));
	if(!IsValid(ResetAttributesEffect))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Reset Attributes Effect. ResetAttributesEffect not set.", true, true, FLinearColor(1,0,0,1), 5.f);
		return;
	}

	if(!IsValid(GetAbilitySystemComponent())) return;

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ResetAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
