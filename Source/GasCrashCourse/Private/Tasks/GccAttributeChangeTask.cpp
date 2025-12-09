// Copyright. All Rights Reserved.


#include "Tasks/GccAttributeChangeTask.h"
#include "AbilitySystemComponent.h"

UGccAttributeChangeTask* UGccAttributeChangeTask::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UGccAttributeChangeTask* WaitForAttributeChangeTask = NewObject<UGccAttributeChangeTask>();
	WaitForAttributeChangeTask->Asc = AbilitySystemComponent;
	WaitForAttributeChangeTask->AttributeToListenFor = Attribute;

	if(!IsValid(AbilitySystemComponent))
	{
		WaitForAttributeChangeTask->RemoveFromRoot();
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &ThisClass::AttributeChanged);

	return WaitForAttributeChangeTask;
}

void UGccAttributeChangeTask::EndTask()
{
	if(Asc.IsValid())
		Asc->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGccAttributeChangeTask::AttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}
