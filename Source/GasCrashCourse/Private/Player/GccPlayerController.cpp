// Copyright. All Rights Reserved.

#include "GasCrashCourse/Public/Player/GccPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/GccTags.h"
#include "Characters/GccBaseCharacter.h"
#include "Utils/DebugUtil.h"

void AGccPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add mapping contexts to the local player's enhanced input subsystem
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(!IsValid(InputSubsystem)) return;

	for(const UInputMappingContext* Context : InputMappingContexts)
		InputSubsystem->AddMappingContext(Context, 0);

	// Bind actions to methods
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if(!IsValid(EnhancedInputComponent)) return;

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &ThisClass::Primary);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::Secondary);
	EnhancedInputComponent->BindAction(TertiaryAction, ETriggerEvent::Started, this, &ThisClass::Tertiary);
}

void AGccPlayerController::Jump()
{
	if(!IsValid(GetCharacter())) return;
	if(!IsAlive()) return;

	GetCharacter()->Jump();
	PRINT_DEBUG("Player Jumped");
}

void AGccPlayerController::StopJumping()
{
	if(!IsValid(GetCharacter())) return;
	if(!IsAlive()) return;

	GetCharacter()->StopJumping();
	PRINT_DEBUG("Player Stopped Jumping");
}

void AGccPlayerController::Move(const FInputActionValue& Value)
{
	if(!IsValid(GetPawn())) return;
	if(!IsAlive()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Find which way is forward
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);

	PRINT_DEBUG("Move input: %s", *MovementVector.ToString());
}

void AGccPlayerController::Look(const FInputActionValue& Value)
{
	if(!bCanMouseLookIfDead)
		if(!IsAlive())
			return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);

	PRINT_DEBUG("Look input: %s", *LookAxisVector.ToString());
}

void AGccPlayerController::Primary()
{
	PRINT_DEBUG("Primary ability key pressed");
	ActivateAbility(GccTags::GccAbilities::Primary);
}

void AGccPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	if(!IsAlive()) return;

	UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if(!IsValid(Asc))
	{
		PRINT_DEBUG_WARNING("ActivateAbility failed: ASC invalid");
		return;
	}

	// ReSharper disable once CppTooWideScope
	const bool bActivated = Asc->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
	if(bActivated)
		PRINT_DEBUG("Activated ability for tag: %s", *AbilityTag.ToString());
	else
		PRINT_DEBUG("No ability activated for tag: %s", *AbilityTag.ToString());
}

void AGccPlayerController::Secondary()
{
	ActivateAbility(GccTags::GccAbilities::Secondary);
	PRINT_DEBUG("Secondary ability requested");
}

void AGccPlayerController::Tertiary()
{
	ActivateAbility(GccTags::GccAbilities::Tertiary);
	PRINT_DEBUG("Tertiary ability requested");
}

bool AGccPlayerController::IsAlive() const
{
	const AGccBaseCharacter* BaseCharacter = Cast<AGccBaseCharacter>(GetPawn());
	if(!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}
