// Copyright. All Rights Reserved.


#include "GasCrashCourse/Public/Player/GccPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/GccTags.h"
#include "Characters/GccBaseCharacter.h"

void AGccPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(!IsValid(InputSubsystem)) return;

	for(const UInputMappingContext* Context : InputMappingContexts)
		InputSubsystem->AddMappingContext(Context, 0);

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
}

void AGccPlayerController::StopJumping()
{
	if(!IsValid(GetCharacter())) return;
	if(!IsAlive()) return;

	GetCharacter()->StopJumping();
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
}

void AGccPlayerController::Look(const FInputActionValue& Value)
{
	if(!bCanMouseLookIfDead)
		if(!IsAlive())
			return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AGccPlayerController::Primary()
{
	ActivateAbility(GccTags::GccAbilities::Primary);
}

void AGccPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	if(!IsAlive()) return;
	UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if(!IsValid(Asc)) return;

	Asc->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

void AGccPlayerController::Secondary()
{
	ActivateAbility(GccTags::GccAbilities::Secondary);
}

void AGccPlayerController::Tertiary()
{
	ActivateAbility(GccTags::GccAbilities::Tertiary);
}

bool AGccPlayerController::IsAlive() const
{
	const AGccBaseCharacter* BaseCharacter = Cast<AGccBaseCharacter>(GetPawn());
	if (!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}

