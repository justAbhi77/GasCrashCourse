// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GccProjectile.generated.h"

class UProjectileMovementComponent;
class UGameplayEffect;

/**
 * Projectile actor used for applying GameplayEffect-based damage and spawning impact FX
 */
UCLASS(Category = "Gas Crash")
class GASCRASHCOURSE_API AGccProjectile : public AActor
{
	GENERATED_BODY()
public:
	AGccProjectile();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Damage amount applied through SetByCaller
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gas Crash|Damage", meta = (ExposeOnSpawn, ClampMin = "1.0"))
	float Damage{25.f};

	// Blueprint FX on hit
	UFUNCTION(BlueprintImplementableEvent, Category = "Gas Crash|Projectile")
	void SpawnImpactEffects();

private:

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, Category = "Gas Crash|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// Damage GameplayEffect
	UPROPERTY(EditDefaultsOnly, Category = "Gas Crash|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
