// Copyright. All Rights Reserved.


#include "AbilitySystem/AbilityTasks/GccWaitGameplayEvent.h"

UGccWaitGameplayEvent* UGccWaitGameplayEvent::WaitGameplayEventToActorProxy(AActor* TargetActor, FGameplayTag EventTag, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UGccWaitGameplayEvent* MyObj = NewObject<UGccWaitGameplayEvent>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tag = EventTag;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;
	return MyObj;
}

void UGccWaitGameplayEvent::StartActivation()
{
	Activate();
}
