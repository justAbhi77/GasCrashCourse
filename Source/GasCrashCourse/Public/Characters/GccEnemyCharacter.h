// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GccBaseCharacter.h"
#include "GccEnemyCharacter.generated.h"


class UAttributeSet;

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API AGccEnemyCharacter : public AGccBaseCharacter
{
	GENERATED_BODY()

public:
	AGccEnemyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UAttributeSet* GetAttributeSet() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|AI")
	float AcceptanceRadius{500.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|AI")
	float MinAttackDelay{.1f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|AI")
	float MaxAttackDelay{.5f};

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
	float GetTimelineLength();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsBeingLaunched{false};

	void StopMovementUntilLanded();
protected:
	virtual void BeginPlay() override;

	virtual void HandleDeath() override;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UFUNCTION()
	void EnableMovementOnLanded(const FHitResult& Hit);
};
