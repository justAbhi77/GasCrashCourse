// Copyright. All Rights Reserved.


#include "Characters/GccEnemyCharacter.h"
#include "AbilitySystem/GccAbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/GccTags.h"


AGccEnemyCharacter::AGccEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGccAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

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

void AGccEnemyCharacter::StopMovementUntilLanded()
{
	bIsBeingLaunched = true;

	AAIController* AIController = GetController<AAIController>();
	if(!IsValid(AIController)) return;

	AIController->StopMovement();

	if(!LandedDelegate.IsAlreadyBound(this, &ThisClass::EnableMovementOnLanded))
		LandedDelegate.AddDynamic(this, &ThisClass::EnableMovementOnLanded);
}

void AGccEnemyCharacter::EnableMovementOnLanded(const FHitResult& Hit)
{
	bIsBeingLaunched = false;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GccTags::Events::Enemy::EndAttack, FGameplayEventData());
	LandedDelegate.RemoveAll(this);
}

void AGccEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);

	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	if(!HasAuthority()) return;

	GiveStartupAbilities();

	InitializeAttributes();

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GccAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void AGccEnemyCharacter::HandleDeath()
{
	Super::HandleDeath();

	AAIController* AIController = GetController<AAIController>();
	if(!IsValid(AIController)) return;
	AIController->StopMovement();
}

UAttributeSet* AGccEnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}
