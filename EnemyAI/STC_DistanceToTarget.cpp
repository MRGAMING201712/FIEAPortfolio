// Fill out your copyright notice in the Description page of Project Settings.


#include "STC_DistanceToTarget.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"

bool FSTC_DistanceToTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& Data = Context.GetInstanceData(*this);

	if (!Data.Actor || !Data.TargetActor)
	{
		return false;
	}

	//Flat distance, ignore verticle
	const float Distance = FVector::Dist2D(Data.Actor->GetActorLocation(), Data.TargetActor->GetActorLocation());

	return Distance >= Data.MinDistance && Distance <= Data.MaxDistance;
}