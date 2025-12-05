// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GccAttributeSet.generated.h"

// Macro to generate standard accessor functions for attributes. Can be used as a macro
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Macro to generate Attribute (let compiler do the heavy lifting :)). Can not be used as a macro in unreal.
 * Use this for copying the preview and delete $ sign.
 */
#define ATTRIBUTE_PROPERTY(PropertyName) \
$UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_##PropertyName, Category = "Attributes") \
$FGameplayAttributeData PropertyName; \
$ATTRIBUTE_ACCESSORS(ThisClass, PropertyName); \
$UFUNCTION() void OnRep_##PropertyName(const FGameplayAttributeData& OldValue) const { $GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PropertyName, OldValue); }

// Macro to simplify replication declaration
#define REGISTER_REPLICATED_ATTRIBUTE(Attribute) \
DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Attribute, COND_None, REPNOTIFY_Always)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributesInitialized);

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintAssignable)
	FAttributesInitialized OnAttributesInitialized;

	UPROPERTY(ReplicatedUsing = OnRep_AttributesInitialized)
	bool bAttributesInitialized = false;

	UFUNCTION()
	void OnRep_AttributesInitialized();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health);

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue) const { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue); }

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);

	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue); }

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);

	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldValue) const { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue); }

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);

	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue); }

	void PostAttributeInitialized();
};
