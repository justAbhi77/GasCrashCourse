// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet.h"
#include "GccWidgetComponent.generated.h"

class UAbilitySystemComponent;
class UGccAttributeSet;
class UGccAbilitySystemComponent;
class AGccBaseCharacter;

/*
* This Widget Component binds gameplay attribute values to UI widgets
* and updates them automatically when attributes change.
*/
UCLASS(ClassGroup=(Custom), Category = "Gas Crash", meta=(BlueprintSpawnableComponent))
class GASCRASHCOURSE_API UGccWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:

	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<AGccBaseCharacter> CrashCharacter;

	TWeakObjectPtr<UGccAbilitySystemComponent> AbilitySystemComponent;

	TWeakObjectPtr<UGccAttributeSet> AttributeSet;

	void InitAbilitySystemData();

	bool IsAscInitialized() const;

	void InitializeAttributeDelegate();

	UFUNCTION()
	void OnAscInitialized(UAbilitySystemComponent* Asc, UAttributeSet* As);

	UFUNCTION()
	void BindToAttributeChanges();

	void BindWidgetToAttributeChanges(UWidget* WidgetObject) const;
};
