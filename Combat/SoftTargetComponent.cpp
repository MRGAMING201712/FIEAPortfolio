// Fill out your copyright notice in the Description page of Project Settings.


#include "SoftTargetComponent.h"
#include "Engine/OverlapResult.h"
#include "EnemyCharacter.h"

// Sets default values for this component's properties
USoftTargetComponent::USoftTargetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USoftTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USoftTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* USoftTargetComponent::FindAttackTarget(FVector PreferredDirection) const
{

	if (!GetOwner() || !GetWorld() || PreferredDirection.IsNearlyZero())
	{
		return nullptr;
	}

	const FVector MyLocation = GetOwner()->GetActorLocation();

	//This flattens so the veritical position doesn't affect who you should hit, might be changed if vertical movement causes issues
	FVector FlatPreferred = PreferredDirection;
	FlatPreferred.Z = 0.0f;
	FlatPreferred = FlatPreferred.GetSafeNormal();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(MaxTargetDistance),
		QueryParams
	);

	TSet<AActor*> Considered;
	AActor* BestTarget = nullptr;
	float BestScore = TNumericLimits<float>::Max(); //makes it the largest value, basically the highest possible positive value so you don't need to worry about assigning a large max starting distance

	const float MaxAngleRadians = FMath::DegreesToRadians(MaxTargetAngle);

	ACharacterBase* MyCharacter = Cast<ACharacterBase>(GetOwner());
	if (!MyCharacter)
	{
		return nullptr;
	}
	const uint8 MyTeam = MyCharacter->GetTeamID();

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* CandidiateActor = Result.GetActor();
		if (!CandidiateActor || Considered.Contains(CandidiateActor))
		{
			continue;
		}

		Considered.Add(CandidiateActor);

		ACharacterBase* Candidate = Cast<ACharacterBase>(CandidiateActor);
		if (!Candidate)
		{
			continue;
		}

		if (Candidate->GetTeamID() == MyTeam)
		{
			continue;   // same team
		}

		if (UActionStateComponent* State = Candidate->GetActionState())
		{
			if (State->GetCurrentState() == ECharacterActionState::Dead)
			{
				continue;
			}
		}

		FVector ToCandidate = CandidiateActor->GetActorLocation() - MyLocation;
		const float Distance = ToCandidate.Size();

		ToCandidate.Z = 0.0f;
		ToCandidate = ToCandidate.GetSafeNormal();

		//Dot gives cos of angle, acos gives radians
		const float Dot = FVector::DotProduct(FlatPreferred, ToCandidate);
		const float AngleRadians = FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)); //Clamp is there to prevent NaN, stupid math mistake

		if (AngleRadians > MaxAngleRadians)
		{
			continue;
		}

		FHitResult LOSHit;
		if (GetWorld()->LineTraceSingleByChannel(LOSHit, MyLocation, CandidiateActor->GetActorLocation(), ECC_Visibility, QueryParams))
		{
			if (LOSHit.GetActor() != CandidiateActor)
			{
				continue; //Something is blocking LOSHit
			}
		}

		const float NormalisedDistance = Distance / MaxTargetDistance;
		const float NormalisedAngle = AngleRadians / MaxAngleRadians;
		const float Score = NormalisedDistance + (NormalisedAngle * AngleWeight);

		if (Score < BestScore)
		{
			BestScore = Score;
			BestTarget = CandidiateActor;
		}
	}

	return BestTarget;
}

