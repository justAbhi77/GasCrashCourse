// Copyright. All Rights Reserved.


#include "AbilitySystem/Abilities/Enemy/GccHitReact.h"

void UGccHitReact::CacheHitDirectionVectors(AActor* Instigator, FVector& OutAvatarForward, FVector& OutToInstigator)
{
	AvatarForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	OutAvatarForward = AvatarForward;

	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector InstigatorLocation = Instigator->GetActorLocation();

	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();
	OutToInstigator = ToInstigator;
}
