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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASCRASHCOURSE_API UGccWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UGccWidgetComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;
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

	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
};
