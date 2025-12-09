// Copyright. All Rights Reserved.

#include "Characters/GccPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/GccPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "GasCrashCourse/GasCrashCourse.h"
#include "Utils/DebugUtil.h"

AGccPlayerCharacter::AGccPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Disable controller-based rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character movement setup
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	// Setup camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Setup follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Add tag to player character for npc's detection algorithms
	Tags.Add(GasCrashTags::Player);
}

UAbilitySystemComponent* AGccPlayerCharacter::GetAbilitySystemComponent() const
{
	const AGccPlayerState* CCPlayerState = Cast<AGccPlayerState>(GetPlayerState());
	if(!IsValid(CCPlayerState))
	{
		PRINT_DEBUG_WARNING("PlayerState invalid in GetAbilitySystemComponent");
		return nullptr;
	}

	return CCPlayerState->GetAbilitySystemComponent();
}

void AGccPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!IsValid(GetAbilitySystemComponent()) || !HasAuthority())
	{
		PRINT_DEBUG_WARNING("AbilitySystemComponent invalid or not authoritative in PossessedBy");
		return;
	}

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	PRINT_DEBUG("ASC initialized on PossessedBy");

	GiveStartupAbilities();
	InitializeAttributes();

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet))
	{
		PRINT_DEBUG_WARNING("AttributeSet invalid in PossessedBy");
		return;
	}

	const FGameplayAttribute HealthAttribute = GccAttributeSet->GetHealthAttribute();
	FOnGameplayAttributeValueChange HealthChangeDelegate = GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(HealthAttribute);
	HealthChangeDelegate.AddUObject(this, &ThisClass::OnHealthChanged);
}

void AGccPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if(!IsValid(GetAbilitySystemComponent()))
	{
		PRINT_DEBUG_WARNING("AbilitySystemComponent invalid in OnRep_PlayerState");
		return;
	}

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet))
	{
		PRINT_DEBUG_WARNING("AttributeSet invalid");
		return;
	}

	const FGameplayAttribute HealthAttribute = GccAttributeSet->GetHealthAttribute();
	FOnGameplayAttributeValueChange HealthChangeDelegate = GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(HealthAttribute);
	HealthChangeDelegate.AddUObject(this, &ThisClass::OnHealthChanged);
}

UAttributeSet* AGccPlayerCharacter::GetAttributeSet() const
{
	const AGccPlayerState* GccPlayerState = Cast<AGccPlayerState>(GetPlayerState());
	if(!IsValid(GccPlayerState))
	{
		PRINT_DEBUG_WARNING("PlayerState invalid in GetAttributeSet");
		return nullptr;
	}

	return GccPlayerState->GetAttributeSet();
}
