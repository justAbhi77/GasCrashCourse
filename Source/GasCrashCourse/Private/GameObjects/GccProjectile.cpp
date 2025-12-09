// Copyright. All Rights Reserved.

#include "GameObjects/GccProjectile.h"
#include "AbilitySystemComponent.h"
#include "Characters/GccPlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/GccTags.h"
#include "Utils/GccBlueprintLibrary.h"
#include "Utils/DebugUtil.h"

AGccProjectile::AGccProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	AActor::SetLifeSpan(10.f);
}

void AGccProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(!IsValid(OtherActor))
	{
		PRINT_DEBUG_WARNING("Overlap but OtherActor is invalid");
		return;
	}

	AGccPlayerCharacter* PlayerCharacter = Cast<AGccPlayerCharacter>(OtherActor);
	if(!IsValid(PlayerCharacter))
	{
		PRINT_DEBUG("Overlap ignored: not a player");
		return;
	}

	if(!PlayerCharacter->IsAlive())
	{
		PRINT_DEBUG("Overlap ignored: player not alive");
		return;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const UAbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetAbilitySystemComponent();
	if(!IsValid(AbilitySystemComponent) || !HasAuthority())
	{
		PRINT_DEBUG_WARNING("Cannot apply damage: ASC invalid or not authority");
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = GetOwner();
	Payload.Target = PlayerCharacter;

	UGccBlueprintLibrary::SendDamageEventToPlayer(PlayerCharacter, DamageEffect, Payload, GccTags::SetByCaller::Projectile, Damage, GccTags::None);

	SpawnImpactEffects();
	PRINT_DEBUG("Impact effects triggered.Projectile destroyed after hit");
	Destroy();
}
