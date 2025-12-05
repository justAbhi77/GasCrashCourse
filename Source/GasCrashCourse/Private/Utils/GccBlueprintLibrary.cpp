// Copyright. All Rights Reserved.


#include "Utils/GccBlueprintLibrary.h"
#include "Characters/GccBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GccAttributeSet.h"
#include "GameplayTags/GccTags.h"
#include "Engine/OverlapResult.h"
#include "Characters/GccEnemyCharacter.h"

EHitDirection UGccBlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);
	if(Dot < -0.5f)
		return EHitDirection::Back;
	if(Dot < 0.5f)
	{
		// Either Left or Right
		if(const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator); Cross.Z < 0.f)
			return EHitDirection::Left;
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

FClosestActorWithTagResult UGccBlueprintLibrary::FindClosestActorWithTag(const UObject* WorldContextObject, const FVector& Origin, const FName& Tag, float SearchRange)
{
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject, Tag, ActorsWithTag);

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

		if(Distance > SearchRange)
			continue;

		if(Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}

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

	FGameplayTag EventTag;
	if(!EventTagOverride.MatchesTagExact(GccTags::None))
		EventTag = EventTagOverride;
	else
	{
		const UGccAttributeSet* AttributeSet = Cast<UGccAttributeSet>(PlayerCharacter->GetAttributeSet());
		if(!IsValid(AttributeSet)) return;

		const bool bLethal = AttributeSet->GetHealth() - Damage <= 0.f;
		EventTag = bLethal ? GccTags::Events::Player::Death : GccTags::Events::Player::HitReact;
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

	// Ensure that the overlap test ignores the Avatar Actor
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0.f, 0.f, HitBoxElevationOffset);

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World)) return TArray<AActor*>();
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere, QueryParams, ResponseParams);

	for(const FOverlapResult& Result : OverlapResults)
	{
		AGccBaseCharacter* BaseCharacter = Cast<AGccBaseCharacter>(Result.GetActor());
		if(!IsValid(BaseCharacter)) continue;
		if(!BaseCharacter->IsAlive()) continue;
		ActorsHit.AddUnique(BaseCharacter);
	}

	if(bDrawDebugs)
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);

	return ActorsHit;
}

void UGccBlueprintLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World)) return;

	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

	for(const FOverlapResult& Result : OverlapResults)
	{
		if(IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(World, DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}

TArray<AActor*> UGccBlueprintLibrary::ApplyKnockback(AActor* AvatarActor, const TArray<AActor*>& HitActors, const float InnerRadius, const float OuterRadius, const float LaunchForceMagnitude, const float RotationAngle, const bool bDrawDebugs)
{
	for(AActor* HitActor : HitActors)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if(!IsValid(HitCharacter) || !IsValid(AvatarActor)) return TArray<AActor*>();

		const FVector HitCharacterLocation = HitCharacter->GetActorLocation();
		const FVector AvatarLocation = AvatarActor->GetActorLocation();

		const FVector ToHitActor = HitCharacterLocation - AvatarLocation;
		const float Distance = FVector::Dist(AvatarLocation, HitCharacterLocation);

		float LaunchForce = 0.f;
		if(Distance > OuterRadius) continue;
		if(Distance <= InnerRadius)
			LaunchForce = LaunchForceMagnitude;
		else
		{
			const FVector2D FalloffRange(InnerRadius, OuterRadius); // input range
			const FVector2D LaunchForceRange(LaunchForceMagnitude, 0.f); // output range
			LaunchForce = FMath::GetMappedRangeValueClamped(FalloffRange, LaunchForceRange, Distance);
		}
		if(bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("LaunchForce: %f"), LaunchForce));

		FVector KnockbackForce = ToHitActor.GetSafeNormal();
		KnockbackForce.Z = 0.f;

		const FVector Right = KnockbackForce.RotateAngleAxis(90.f, FVector::UpVector);
		KnockbackForce = KnockbackForce.RotateAngleAxis(-RotationAngle, Right) * LaunchForce;

		if(bDrawDebugs)
		{
			const UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
			DrawDebugDirectionalArrow(World, HitCharacterLocation, HitCharacterLocation + KnockbackForce, 100.f, FColor::Green, false, 3.f);
		}

		if(AGccEnemyCharacter* EnemyCharacter = Cast<AGccEnemyCharacter>(HitCharacter); IsValid(EnemyCharacter))
			EnemyCharacter->StopMovementUntilLanded();

		HitCharacter->LaunchCharacter(KnockbackForce, true, true);
	}
	return HitActors;
}
