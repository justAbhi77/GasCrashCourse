// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GccMeleeAttack.generated.h"

/**
 *
 */
UCLASS()
class GASCRASHCOURSE_API UGccMeleeAttack : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
private:
	UPROPERTY(EditAnywhere, Category = "GasCrash|Debugs")
	bool bDrawDebugs = true;

	UPROPERTY(EditAnywhere, Category = "GasCrash|Socket")
	FName SocketName{"Socket_MeleeWeapon"};

	UPROPERTY(EditAnywhere, Category = "GasCrash|Socket")
	float SocketExtensionOffset{40.f};

	UPROPERTY(EditAnywhere, Category = "GasCrash|Socket")
	float SphereTraceRadius{60.f};

	TArray<FHitResult> PerformSphereTrace(USkeletalMeshComponent* MeshComp) const;

	void SendEventToActors(const USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits);

	UPROPERTY()
	TArray<AActor*> UniquePlayerChars;
};
