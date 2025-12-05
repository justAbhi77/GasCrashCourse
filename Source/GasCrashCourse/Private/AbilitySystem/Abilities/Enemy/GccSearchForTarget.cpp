// Copyright. All Rights Reserved.


#include "AbilitySystem/Abilities/Enemy/GccSearchForTarget.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/AbilityTasks/GccWaitGameplayEvent.h"
#include "Characters/GccEnemyCharacter.h"
#include "GameplayTags/GccTags.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/GccBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGccSearchForTarget::UGccSearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGccSearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OwningEnemy = Cast<AGccEnemyCharacter>(GetAvatarActorFromActorInfo());
	// check(OwningEnemy.IsValid());
	if(!OwningEnemy.IsValid())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Owning Enemy. OwningEnemy not set.", true, true, FLinearColor(1,0,0,1), 5.f);
		return;
	}
	OwningAIController = Cast<AAIController>(OwningEnemy->GetController());
	// check(OwningAIController.IsValid());
	if(!OwningAIController.IsValid())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Owning AI Controller. OwningAIController not set.", true, true, FLinearColor(1,0,0,1), 5.f);
		return;
	}

	StartSearch();

	WaitGameplayEventTask = UGccWaitGameplayEvent::WaitGameplayEventToActorProxy(GetAvatarActorFromActorInfo(), GccTags::Events::Enemy::EndAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::EndAttackEventReceived);
	WaitGameplayEventTask->StartActivation();
}

void UGccSearchForTarget::StartSearch()
{
	if(bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("UGccSearchForTarget::StartSearch")));
	if(!OwningEnemy.IsValid()) return;

	const float SearchDelay = FMath::RandRange(OwningEnemy->MinAttackDelay, OwningEnemy->MaxAttackDelay);
	SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SearchDelay);
	SearchDelayTask->OnFinish.AddDynamic(this, &ThisClass::Search);
	SearchDelayTask->Activate();
}

void UGccSearchForTarget::EndAttackEventReceived(FGameplayEventData Payload)
{
	if(OwningEnemy.IsValid() && !OwningEnemy->bIsBeingLaunched)
		StartSearch();
}

void UGccSearchForTarget::Search()
{
	const FVector SearchOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();

	if(!OwningEnemy.IsValid()) return;
	const auto [ClosestActor, ClosestDistance] = UGccBlueprintLibrary::FindClosestActorWithTag(this, SearchOrigin, GasCrashTags::Player, OwningEnemy->SearchRange);

	TargetBaseCharacter = Cast<AGccBaseCharacter>(ClosestActor);

	if(!TargetBaseCharacter.IsValid())
	{
		StartSearch();
		return;
	}

	if(TargetBaseCharacter->IsAlive())
		MoveToTargetAndAttack();
	else
		StartSearch();
}

void UGccSearchForTarget::MoveToTargetAndAttack()
{
	if(!OwningEnemy.IsValid() || !OwningAIController.IsValid() || !TargetBaseCharacter.IsValid()) return;
	if(!OwningEnemy->IsAlive())
	{
		StartSearch();
		return;
	}

	MoveToLocationOrActorTask = UAITask_MoveTo::AIMoveTo(OwningAIController.Get(), FVector(), TargetBaseCharacter.Get(), OwningEnemy->AcceptanceRadius);
	MoveToLocationOrActorTask->OnMoveTaskFinished.AddUObject(this, &ThisClass::AttackTarget);
	MoveToLocationOrActorTask->ConditionalPerformMove();
}

void UGccSearchForTarget::AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if(Result != EPathFollowingResult::Success)
	{
		StartSearch();
		return;
	}
	OwningEnemy->RotateToTarget(TargetBaseCharacter.Get());

	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimelineLength());
	AttackDelayTask->OnFinish.AddDynamic(this, &ThisClass::Attack);
	AttackDelayTask->Activate();
}

void UGccSearchForTarget::Attack()
{
	if(!OwningEnemy.IsValid()) return;

	if(!OwningEnemy->IsAlive())
	{
		StartSearch();
		return;
	}

	const FGameplayTag AttackTag = GccTags::GccAbilities::Enemy::Attack;
	if(!GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer()))
	{
		StartSearch();
		return;
	}
}