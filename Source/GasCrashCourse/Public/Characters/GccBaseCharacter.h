// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GasCrashCourse/GasCrashCourse.h"
#include "GccBaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAscInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

/**
 *
 */
UCLASS(Abstract)
class GASCRASHCOURSE_API AGccBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGccBaseCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	UPROPERTY(BlueprintAssignable)
	FAscInitialized OnAscInitialized;

	bool IsAlive() const { return bAlive; }
	void SetAlive(const bool bAliveStatus) { bAlive = bAliveStatus; }

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Death")
	virtual void HandleRespawn();

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Attributes")
	void ResetAttributes() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RotateToTarget(AActor* RotateTarget);

	UPROPERTY(EditAnywhere, Category = "GasCrash|AI")
	float SearchRange{1000.f};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GasCrash|Damage")
	float DamageNumberVerticalOffset{200.f};
protected:
	void GiveStartupAbilities();

	void InitializeAttributes() const;

	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);
	virtual void HandleDeath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldRespawn = false;
private:
	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;
};
