// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GccBaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAscInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

/**
 *	Base character supporting ASC, attributes and death handling
 */
UCLASS(Abstract, Category = "Gas Crash")
class GASCRASHCOURSE_API AGccBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGccBaseCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Virtual function from IAbilitySystemInterface to get the component
	 * @return The Ability System Component associated with this actor
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	UPROPERTY(BlueprintAssignable)
	FAscInitialized OnAscInitialized;

	bool IsAlive() const { return bAlive; }
	void SetAlive(const bool bAliveStatus) { bAlive = bAliveStatus; }

	UFUNCTION(BlueprintCallable, Category = "Gas Crash|Death")
	virtual void HandleRespawn();

	UFUNCTION(BlueprintCallable, Category = "Gas Crash|Attributes")
	void ResetAttributes() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RotateToTarget(AActor* RotateTarget);

	UPROPERTY(EditAnywhere, Category = "Gas Crash|AI")
	float SearchRange{1000.f};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gas Crash|Damage")
	float DamageNumberVerticalOffset{200.f};

protected:

	void GiveStartupAbilities();

	void InitializeAttributes() const;

	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);

	virtual void HandleDeath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldRespawn = false;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bAlive = true;

	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;
};
