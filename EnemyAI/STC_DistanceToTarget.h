// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STC_DistanceToTarget.generated.h"

USTRUCT()
struct FSTC_DistanceToTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MinDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MaxDistance = 200.0f;
};

USTRUCT(meta = (DisplayName = "Distance To Target"))
struct FIEAPORTFOLIO_API FSTC_DistanceToTarget : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTC_DistanceToTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};