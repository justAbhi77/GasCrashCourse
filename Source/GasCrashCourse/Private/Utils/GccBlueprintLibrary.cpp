// Copyright. All Rights Reserved.

#include "Utils/GccBlueprintLibrary.h"
#include "Characters/GccBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "GameplayTags/GccTags.h"
#include "Engine/OverlapResult.h"
#include "Characters/GccEnemyCharacter.h"
#include "Utils/DebugUtil.h"

EHitDirection UGccBlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);

	if(Dot < -0.5f) return EHitDirection::Back;

	if(Dot < 0.5f)
	{
		// ReSharper disable once CppTooWideScopeInitStatement
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if(Cross.Z < 0.f) return EHitDirection::Left;
		return EHitDirection::Right;
	}

	return EHitDirection::Forward;
}

FName UGccBlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch(HitDirection)
	{
		case EHitDirection::Left: return FName("Left");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Forward: return FName("Forward");
		case EHitDirection::Back: return FName("Back");
		default: return FName("None");
	}
}

FClosestActorWithTagResult UGccBlueprintLibrary::FindClosestActorWithTag(const UObject* WorldContextObject, const FVector& Origin, const FName& Tag, const float SearchRange)
{
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject, Tag, ActorsWithTag);

	PRINT_DEBUG("Searching for closest actor with Tag '%s' in range %f", *Tag.ToString(), SearchRange);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	for(AActor* Actor : ActorsWithTag)
	{
		if(!IsValid(Actor)) continue;

		// ReSharper disable once CppTooWideScopeInitStatement
		const AGccBaseCharacter* BaseCharacter = Cast<AGccBaseCharacter>(Actor);
		if(!IsValid(BaseCharacter) || !BaseCharacter->IsAlive()) continue;

		// ReSharper disable once CppTooWideScopeInitStatement
		const float Distance = FVector::Dist(Origin, Actor->GetActorLocation());
		if(Distance > SearchRange) continue;

		if(Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}

	if(IsValid(ClosestActor))
		PRINT_DEBUG("Closest actor: %s at distance %.2f", *ClosestActor->GetName(), ClosestDistance);
	else
		PRINT_DEBUG_WARNING("No valid actor with Tag '%s' found in range", *Tag.ToString());

	FClosestActorWithTagResult Result;
	Result.Actor = ClosestActor;
	Result.Distance = ClosestDistance;
	return Result;
}

void UGccBlueprintLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayEventData& Payload, const FGameplayTag& DataTag, const float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	AGccBaseCharacter* PlayerCharacter = Cast<AGccBaseCharacter>(Target);
	if(!IsValid(PlayerCharacter)) return;
	if(!PlayerCharacter->IsAlive()) return;

	PRINT_DEBUG("Sending damage event to %s. Damage: %f", *PlayerCharacter->GetName(), Damage);

	FGameplayTag EventTag;
	if(!EventTagOverride.MatchesTagExact(GccTags::None))
		EventTag = EventTagOverride;
	else
	{
		const UGccAttributeSet* Attr = Cast<UGccAttributeSet>(PlayerCharacter->GetAttributeSet());
		if(!IsValid(Attr)) return;

		const bool bLethal = Attr->GetHealth() - Damage <= 0.f;
		EventTag = bLethal ? GccTags::Events::Player::Death : GccTags::Events::Player::HitReact;

		PRINT_DEBUG("Computed event tag: %s", *EventTag.ToString());
	}

	Payload.OptionalObject = OptionalParticleSystem;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter, EventTag, Payload);

	UAbilitySystemComponent* TargetAsc = PlayerCharacter->GetAbilitySystemComponent();
	if(!IsValid(TargetAsc)) return;

	const FGameplayEffectContextHandle ContextHandle = TargetAsc->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = TargetAsc->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, -Damage);

	TargetAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGccBlueprintLibrary::SendDamageEventToPlayers(TArray<AActor*> Targets,const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayEventData& Payload, const FGameplayTag& DataTag, const float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	for(AActor* Target : Targets)
		SendDamageEventToPlayer(Target, DamageEffect, Payload, DataTag, Damage, EventTagOverride, OptionalParticleSystem);
}

TArray<AActor*> UGccBlueprintLibrary::HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	TArray<AActor*> ActorsHit;
	if(!IsValid(AvatarActor)) return ActorsHit;

	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0.f, 0.f, HitBoxElevationOffset);

	PRINT_DEBUG("HitBox test at location %s radius %.2f", *HitBoxLocation.ToString(), HitBoxRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World)) return ActorsHit;

	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere, QueryParams, ResponseParams);

	for(const FOverlapResult& Result : OverlapResults)
	{
		// ReSharper disable once CppTooWideScopeInitStatement
		AGccBaseCharacter* HitChar = Cast<AGccBaseCharacter>(Result.GetActor());
		if(!IsValid(HitChar) || !HitChar->IsAlive()) continue;

		ActorsHit.AddUnique(HitChar);
	}

	if(bDrawDebugs)
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);

	return ActorsHit;
}

void UGccBlueprintLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, const float HitBoxRadius)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World)) return;

	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

	for(const FOverlapResult& Result : OverlapResults)
	{
		if(IsValid(Result.GetActor()))
		{
			FVector Loc = Result.GetActor()->GetActorLocation() + FVector(0.f, 0.f, 100.f);
			DrawDebugSphere(World, Loc, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}

TArray<AActor*> UGccBlueprintLibrary::ApplyKnockback(AActor* AvatarActor, const TArray<AActor*>& HitActors, const float InnerRadius, const float OuterRadius, const float LaunchForceMagnitude, const float RotationAngle, const bool bDrawDebugs)
{
	for(AActor* HitActor : HitActors)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if(!IsValid(HitCharacter) || !IsValid(AvatarActor)) return TArray<AActor*>();

		const FVector ToHit = HitCharacter->GetActorLocation() - AvatarActor->GetActorLocation();
		const float Distance = ToHit.Size();

		if(Distance > OuterRadius) continue;

		float LaunchForce;
		if(Distance <= InnerRadius)
			LaunchForce = LaunchForceMagnitude;
		else
		{
			const FVector2D FalloffRange(InnerRadius, OuterRadius); // input range
			const FVector2D LaunchForceRange(LaunchForceMagnitude, 0.f); // output range
			LaunchForce = FMath::GetMappedRangeValueClamped(FalloffRange, LaunchForceRange, Distance);
		}

		FVector KnockbackForce = ToHit.GetSafeNormal();
		KnockbackForce.Z = 0.f;

		const FVector Right = KnockbackForce.RotateAngleAxis(90.f, FVector::UpVector);
		KnockbackForce = KnockbackForce.RotateAngleAxis(-RotationAngle, Right) * LaunchForce;

		if(bDrawDebugs)
		{
			const UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
			DrawDebugDirectionalArrow(World, HitCharacter->GetActorLocation(), HitCharacter->GetActorLocation() + KnockbackForce, 100.f, FColor::Green, false, 3.f);
		}

		if(AGccEnemyCharacter* Enemy = Cast<AGccEnemyCharacter>(HitCharacter); IsValid(Enemy))
			Enemy->StopMovementUntilLanded();

		HitCharacter->LaunchCharacter(KnockbackForce, true, true);
	}

	return HitActors;
}
