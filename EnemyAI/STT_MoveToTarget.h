#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_MoveToTarget.generated.h"

USTRUCT()
struct FSTT_MoveToTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 150.0f;

	//Need to have new paths so they don't clump up
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float RingRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter") 
	float RepathInterval = 0.5f;

	UPROPERTY()
	float RepathElapsed = 0.0f;
};

USTRUCT(meta = (DisplayName = "Move To Target"))
struct FIEAPORTFOLIO_API FSTT_MoveToTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_MoveToTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	protected:
	bool MoveToRingPosition(FInstanceDataType& Data) const;
};