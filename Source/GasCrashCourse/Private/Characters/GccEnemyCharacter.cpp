// Copyright. All Rights Reserved.

#include "Characters/GccEnemyCharacter.h"
#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "GameplayTags/GccTags.h"
#include "Net/UnrealNetwork.h"
#include "Utils/DebugUtil.h"

AGccEnemyCharacter::AGccEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create ASC
	AbilitySystemComponent = CreateDefaultSubobject<UGccAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Create AttributeSet
	AttributeSet = CreateDefaultSubobject<UGccAttributeSet>("AttributeSet");
}

void AGccEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsBeingLaunched);
}

UAbilitySystemComponent* AGccEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AGccEnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void AGccEnemyCharacter::StopMovementUntilLanded()
{
	bIsBeingLaunched = true;
	PRINT_DEBUG("Enemy launched");

	AAIController* AIController = GetController<AAIController>();
	if(!IsValid(AIController))
	{
		PRINT_DEBUG_WARNING("StopMovementUntilLanded failed: No AIController");
		return;
	}

	AIController->StopMovement();

	if(!LandedDelegate.IsAlreadyBound(this, &ThisClass::EnableMovementOnLanded))
		LandedDelegate.AddDynamic(this, &ThisClass::EnableMovementOnLanded);
}

void AGccEnemyCharacter::EnableMovementOnLanded(const FHitResult& Hit)
{
	bIsBeingLaunched = false;
	PRINT_DEBUG("Enemy landed");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GccTags::Events::Enemy::EndAttack, FGameplayEventData());
	LandedDelegate.RemoveAll(this);
}

void AGccEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(!IsValid(GetAbilitySystemComponent()))
	{
		PRINT_DEBUG_WARNING("ASC invalid in BeginPlay");
		return;
	}

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	if(!HasAuthority())
	{
		PRINT_DEBUG("Client-side BeginPlay complete");
		return;
	}

	GiveStartupAbilities();
	InitializeAttributes();

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet))
	{
		PRINT_DEBUG_WARNING("AttributeSet invalid during health binding");
		return;
	}

	const FGameplayAttribute HealthAttribute = GccAttributeSet->GetHealthAttribute();
	FOnGameplayAttributeValueChange HealthChangeDelegate = GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(HealthAttribute);
	HealthChangeDelegate.AddUObject(this, &ThisClass::OnHealthChanged);
}

void AGccEnemyCharacter::HandleDeath()
{
	Super::HandleDeath();
	PRINT_DEBUG("Enemy died, stopping movement");

	AAIController* AIController = GetController<AAIController>();
	if(!IsValid(AIController))
	{
		PRINT_DEBUG_WARNING("HandleDeath: No AIController");
		return;
	}

	AIController->StopMovement();
}
