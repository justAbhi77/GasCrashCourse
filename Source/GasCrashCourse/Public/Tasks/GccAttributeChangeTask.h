// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AttributeSet.h"
#include "GccAttributeChangeTask.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 *
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASCRASHCOURSE_API UGccAttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGccAttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	TWeakObjectPtr<UAbilitySystemComponent> Asc;
	FGameplayAttribute AttributeToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};
