// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GccBlueprintLibrary.generated.h"


struct FGameplayTag;
struct FGameplayEventData;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Left,
	Right,
	Forward,
	Back
};

USTRUCT(BlueprintType)
struct FClosestActorWithTagResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY(BlueprintReadWrite)
	float Distance{0.f};
};

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "GasCrash")
	static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

	UFUNCTION(BlueprintPure, Category = "GasCrash")
	static FName GetHitDirectionName(const EHitDirection& HitDirection);

	UFUNCTION(BlueprintCallable, Category = "GasCrash", meta = (DefaultToSelf = "WorldContextObject"))
	static FClosestActorWithTagResult FindClosestActorWithTag(const UObject* WorldContextObject, const FVector& Origin, const FName& Tag, float SearchRange);

	UFUNCTION(BlueprintCallable, Category = "GasCrash")
	static void SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect, UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);

	UFUNCTION(BlueprintCallable)
	static void SendDamageEventToPlayers(TArray<AActor*> Targets, const TSubclassOf<UGameplayEffect>& DamageEffect, UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Abilities")
	static TArray<AActor*> HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset = 0.f, float HitBoxElevationOffset = 0.f, bool bDrawDebugs = false);

	static void DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius);

	UFUNCTION(BlueprintCallable, Category = "GasCrash|Abilities")
	static TArray<AActor*> ApplyKnockback(AActor* AvatarActor, const TArray<AActor*>& HitActors, float InnerRadius, float OuterRadius, float LaunchForceMagnitude, float RotationAngle = 45.f, bool bDrawDebugs = false);
};
