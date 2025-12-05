// Copyright. All Rights Reserved.


#include "Notifies/GccMeleeAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "KismetTraceUtils.h"
#include "Characters/GccPlayerCharacter.h"
#include "GameplayTags/GccTags.h"
#include "Kismet/KismetMathLibrary.h"

void UGccMeleeAttack::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UniquePlayerChars.Empty();
}

void UGccMeleeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if(!IsValid(MeshComp)) return;
	if(!IsValid(MeshComp->GetOwner())) return;

	const TArray<FHitResult> Hits = PerformSphereTrace(MeshComp);
	SendEventToActors(MeshComp, Hits);
}

TArray<FHitResult> UGccMeleeAttack::PerformSphereTrace(USkeletalMeshComponent* MeshComp) const
{
	TArray<FHitResult> OutHits;

	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
	const FVector Start = SocketTransform.GetLocation();
	const FVector ExtendedSocketDirection = UKismetMathLibrary::GetForwardVector(SocketTransform.GetRotation().Rotator()) * SocketExtensionOffset;
	const FVector End = Start - ExtendedSocketDirection;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	UWorld* World = GEngine->GetWorldFromContextObject(MeshComp, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World)) return OutHits;

	bool const bHit = World->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereTraceRadius), Params, ResponseParams);

	if(bDrawDebugs)
		DrawDebugSphereTraceMulti(World, Start, End, SphereTraceRadius, EDrawDebugTrace::ForDuration, bHit, OutHits, FColor::Red, FColor::Green, 5.f);

	return OutHits;
}

void UGccMeleeAttack::SendEventToActors(const USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits)
{
	for(const FHitResult& Hit : Hits)
	{
		if(UniquePlayerChars.Contains(Hit.GetActor())) continue;
			UniquePlayerChars.Add(Hit.GetActor());

		AGccPlayerCharacter* PlayerCharacter = Cast<AGccPlayerCharacter>(Hit.GetActor());
		if(!IsValid(PlayerCharacter)) continue;
		if(!PlayerCharacter->IsAlive()) continue;

		const UAbilitySystemComponent* Asc = PlayerCharacter->GetAbilitySystemComponent();
		if(!IsValid(Asc)) continue;

		FGameplayEffectContextHandle ContextHandle = Asc->MakeEffectContext();
		ContextHandle.AddHitResult(Hit);

		FGameplayEventData Payload;
		Payload.Target = PlayerCharacter;
		Payload.ContextHandle = ContextHandle;
		Payload.Instigator = MeshComp->GetOwner();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), GccTags::Events::Enemy::MeleeTraceHit, Payload);
	}
}