#include "STT_MoveToTarget.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "CharacterBase.h"
#include "NavigationSystem.h"

EStateTreeRunStatus FSTT_MoveToTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.RepathElapsed = 0.0f;

	if (!MoveToRingPosition(Data))
	{
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_MoveToTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);

	APawn* Pawn = Cast<APawn>(Data.Actor);
	if (!Pawn || !Data.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Close enough to the player to attack, regardless of ring position
	const float DistanceToTarget = FVector::Dist2D(
		Pawn->GetActorLocation(), Data.TargetActor->GetActorLocation());

	if (DistanceToTarget <= Data.RingRadius + Data.AcceptanceRadius)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	// The player moves, so the ring moves, reissue periodically
	Data.RepathElapsed += DeltaTime;
	if (Data.RepathElapsed >= Data.RepathInterval)
	{
		Data.RepathElapsed = 0.0f;
		MoveToRingPosition(Data);
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_MoveToTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& Data = Context.GetInstanceData(*this);

	if (APawn* Pawn = Cast<APawn>(Data.Actor))
	{
		if (AAIController* AI = Cast<AAIController>(Pawn->GetController()))
		{
			AI->StopMovement();
		}
	}
}

bool FSTT_MoveToTarget::MoveToRingPosition(FInstanceDataType& Data) const
{
	APawn* Pawn = Cast<APawn>(Data.Actor);
	if (!Pawn || !Data.TargetActor)
	{
		return false;
	}

	AAIController* AI = Cast<AAIController>(Pawn->GetController());
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Pawn->GetWorld());
	if (!AI || !Nav)
	{
		return false;
	}

	// Each enemy has its own slice of the ring, so a dozen of them have a dozen destinations rather than converging on one point
	float AngleDegrees = 0.0f;
	if (ACharacterBase* Character = Cast<ACharacterBase>(Data.Actor))
	{
		AngleDegrees = Character->GetOrbitAngleOffset();
	}

	const FVector Center = Data.TargetActor->GetActorLocation();

	for (int32 Attempt = 0; Attempt < 4; ++Attempt)
	{
		const float Radians = FMath::DegreesToRadians(AngleDegrees);
		const FVector Offset(FMath::Cos(Radians) * Data.RingRadius,
			FMath::Sin(Radians) * Data.RingRadius, 0.0f);

		FNavLocation Projected;
		if (Nav->ProjectPointToNavigation(Center + Offset, Projected, FVector(150.0f)))
		{
			AI->MoveToLocation(Projected.Location, Data.AcceptanceRadius);
			return true;
		}

		AngleDegrees += 45.0f;   // that slice is blocked, try the next
	}

	// Nothing on the ring worked, fall back to the target itself
	AI->MoveToActor(Data.TargetActor, Data.AcceptanceRadius);
	return true;
}