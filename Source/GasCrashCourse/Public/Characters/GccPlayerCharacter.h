// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GccBaseCharacter.h"
#include "GccPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API AGccPlayerCharacter : public AGccBaseCharacter
{
	GENERATED_BODY()

public:
	AGccPlayerCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual UAttributeSet* GetAttributeSet() const override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
};
