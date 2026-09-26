#include "STT_OrbitTarget.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "EnemyCharacter.h"

EStateTreeRunStatus FSTT_OrbitTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);

	APawn* Pawn = Cast<APawn>(Data.Actor);
	if (!Pawn || !Data.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	//Start from this enemy's offset so the rest circle up correctly
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Data.Actor))
	{
		Data.CurrentAngleDegrees = Enemy->GetOrbitAngleOffset();
	}
	else
	{
		Data.CurrentAngleDegrees = FMath::FRandRange(0.0f, 360.0f);
	}

	Data.bWaiting = false;
	Data.PauseElapsed = 0.0f;

	PickOrbitPoint(Data);
	return EStateTreeRunStatus::Running;
}

bool FSTT_OrbitTarget::PickOrbitPoint(FInstanceDataType& Data) const
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

	const FVector Centre = Data.TargetActor->GetActorLocation();

	//Could land off the nav mesh so check a few other angles as well
	for (int32 Attempt = 0; Attempt < 4; ++Attempt)
	{
		const float Radians = FMath::DegreesToRadians(Data.CurrentAngleDegrees);
		const FVector Offset(FMath::Cos(Radians) * Data.OrbitRadius, FMath::Sin(Radians) * Data.OrbitRadius, 0.0f);

		FNavLocation Projected;
		if (Nav->ProjectPointToNavigation(Centre + Offset, Projected, FVector(100.0f)))
		{
			AI->MoveToLocation(Projected.Location, 50.0f);
			return true;
		}

		Data.CurrentAngleDegrees += Data.StepAngle;
	}

	return false;
}

EStateTreeRunStatus FSTT_OrbitTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);

	APawn* Pawn = Cast<APawn>(Data.Actor);
	if (!Pawn || !Data.TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	//Face the target
	FVector ToTarget = Data.TargetActor->GetActorLocation() - Pawn->GetActorLocation();
	ToTarget.Z = 0.0f;
	if (!ToTarget.IsNearlyZero())
	{
		Pawn->SetActorRotation(ToTarget.Rotation());
	}

	if (Data.bWaiting)
	{
		Data.PauseElapsed += DeltaTime;
		if (Data.PauseElapsed >= Data.PauseBetweenSteps)
		{
			Data.bWaiting = false;
			Data.PauseElapsed = 0.0f;
			Data.CurrentAngleDegrees += Data.StepAngle;
			PickOrbitPoint(Data);
		}
		return EStateTreeRunStatus::Running;
	}

	Data.MoveElapsed += DeltaTime;
	if (Data.MoveElapsed > Data.MoveTimeout)
	{
		Data.MoveElapsed = 0.0f;
		Data.CurrentAngleDegrees += Data.StepAngle;
		PickOrbitPoint(Data);
	}

	if (AAIController* AI = Cast<AAIController>(Pawn->GetController()))
	{
		if (!AI->IsFollowingAPath())
		{
			Data.bWaiting = true;
		}
	}

	//Orbiting ends once it leaves this state
	return EStateTreeRunStatus::Running;
}

void FSTT_OrbitTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
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