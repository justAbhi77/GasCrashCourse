// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GccProjectile.generated.h"

class UProjectileMovementComponent;
class UGameplayEffect;

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API AGccProjectile : public AActor
{
	GENERATED_BODY()
public:
	AGccProjectile();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GasCrash|Damage", meta = (ExposeOnSpawn, ClampMin = "1.0"))
	float Damage{25.f};

	UFUNCTION(BlueprintImplementableEvent, Category = "GasCrash|Projectile")
	void SpawnImpactEffects();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "GasCrash|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "GasCrash|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
