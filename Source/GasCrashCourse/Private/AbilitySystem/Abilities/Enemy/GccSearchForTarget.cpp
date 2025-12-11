// Copyright. All Rights Reserved.

#include "AbilitySystem/Abilities/Enemy/GccSearchForTarget.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/AbilityTasks/GccWaitGameplayEvent.h"
#include "Characters/GccEnemyCharacter.h"
#include "GameplayTags/GccTags.h"
#include "GasCrashCourse/GasCrashCourse.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/GccBlueprintLibrary.h"
#include "Utils/DebugUtil.h"

UGccSearchForTarget::UGccSearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGccSearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OwningEnemy = Cast<AGccEnemyCharacter>(GetAvatarActorFromActorInfo());
	if(!OwningEnemy.IsValid())
	{
		PRINT_DEBUG_WARNING("Invalid Owning Enemy. OwningEnemy not set.");
		return;
	}

	OwningAIController = Cast<AAIController>(OwningEnemy->GetController());
	if(!OwningAIController.IsValid())
	{
		PRINT_DEBUG_WARNING("Invalid AIController");
		return;
	}

	StartSearch();

	WaitGameplayEventTask = UGccWaitGameplayEvent::WaitGameplayEventToActorProxy(GetAvatarActorFromActorInfo(), GccTags::Events::Enemy::EndAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::EndAttackEventReceived);
	WaitGameplayEventTask->StartActivation();
}

void UGccSearchForTarget::StartSearch()
{
	PRINT_DEBUG("");

	if(!OwningEnemy.IsValid()) return;

	const float SearchDelay = FMath::RandRange(OwningEnemy->MinAttackDelay, OwningEnemy->MaxAttackDelay);

	SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SearchDelay);
	SearchDelayTask->OnFinish.AddDynamic(this, &ThisClass::Search);
	SearchDelayTask->Activate();
}

void UGccSearchForTarget::EndAttackEventReceived(FGameplayEventData Payload)
{
	PRINT_DEBUG("EndAttack Event Received");

	if(OwningEnemy.IsValid() && !OwningEnemy->bIsBeingLaunched) StartSearch();
}

void UGccSearchForTarget::Search()
{
	if(!OwningEnemy.IsValid()) return;

	const FVector SearchOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();

	const auto [ClosestActor, ClosestDistance] = UGccBlueprintLibrary::FindClosestActorWithTag(this, SearchOrigin, GasCrashTags::Player, OwningEnemy->SearchRange);
	TargetBaseCharacter = Cast<AGccBaseCharacter>(ClosestActor);

	if(!TargetBaseCharacter.IsValid())
	{
		StartSearch();
		return;
	}

	PRINT_DEBUG("Target found: %s", *TargetBaseCharacter->GetName());

	if(TargetBaseCharacter->IsAlive()) MoveToTargetAndAttack();
	else StartSearch();
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

	PRINT_DEBUG("Attempting attack");

	// ReSharper disable once CppTooWideScopeInitStatement
	const FGameplayTag AttackTag = GccTags::GccAbilities::Enemy::Attack;

	if(!GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer()))
	{
		StartSearch();
		return;
	}
}
