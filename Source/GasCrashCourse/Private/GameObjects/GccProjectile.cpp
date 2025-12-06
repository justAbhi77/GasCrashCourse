// Copyright. All Rights Reserved.


#include "GameObjects/GccProjectile.h"
#include "AbilitySystemComponent.h"
#include "Characters/GccPlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/GccTags.h"
#include "Utils/GccBlueprintLibrary.h"


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

	AGccPlayerCharacter* PlayerCharacter = Cast<AGccPlayerCharacter>(OtherActor);
	if(!IsValid(PlayerCharacter)) return;
	if(!PlayerCharacter->IsAlive()) return;
	if(const UAbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetAbilitySystemComponent(); !IsValid(AbilitySystemComponent) || !HasAuthority()) return;

	FGameplayEventData Payload;
	Payload.Instigator = GetOwner();
	Payload.Target = PlayerCharacter;

	UGccBlueprintLibrary::SendDamageEventToPlayer(PlayerCharacter, DamageEffect, Payload, GccTags::SetByCaller::Projectile, Damage, GccTags::None);

	SpawnImpactEffects();
	Destroy();
}

void AGccProjectile::BeginPlay()
{
	Super::BeginPlay();
}

