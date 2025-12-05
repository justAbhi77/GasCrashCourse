// Copyright. All Rights Reserved.


#include "GasCrashCourse/Public/Characters/GccPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/GccPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GccAttributeSet.h"


AGccPlayerCharacter::AGccPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Tags.Add(GasCrashTags::Player);
}


UAbilitySystemComponent* AGccPlayerCharacter::GetAbilitySystemComponent() const
{
	const AGccPlayerState* CCPlayerState = Cast<AGccPlayerState>(GetPlayerState());
	if(!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAbilitySystemComponent();
}

void AGccPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);

	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	GiveStartupAbilities();

	InitializeAttributes();

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GccAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void AGccPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if(!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);

	OnAscInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	const UGccAttributeSet* GccAttributeSet = Cast<UGccAttributeSet>(GetAttributeSet());
	if(!IsValid(GccAttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GccAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

UAttributeSet* AGccPlayerCharacter::GetAttributeSet() const
{
	const AGccPlayerState* GccPlayerState = Cast<AGccPlayerState>(GetPlayerState());
	if(!IsValid(GccPlayerState)) return nullptr;

	return GccPlayerState->GetAttributeSet();
}
