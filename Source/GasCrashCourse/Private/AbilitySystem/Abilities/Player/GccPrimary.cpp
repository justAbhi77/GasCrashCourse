// Copyright. All Rights Reserved.


#include "AbilitySystem/Abilities/Player/GccPrimary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/GccTags.h"

void UGccPrimary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit) const
{
	for(AActor* HitActor : ActorsHit)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, GccTags::Events::Enemy::HitReact, Payload);
	}
}
