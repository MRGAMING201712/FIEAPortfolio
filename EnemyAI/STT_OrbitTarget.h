#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_OrbitTarget.generated.h"

USTRUCT()
struct FSTT_OrbitTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float OrbitRadius = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float StepAngle = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float PauseBetweenSteps = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MoveTimeout = 3.0f;

	//Runtime state, stuff when it starts
	UPROPERTY()
	float CurrentAngleDegrees = 0.0f;

	UPROPERTY()
	float PauseElapsed = 0.0f;

	UPROPERTY()
	bool bWaiting = false;

	UPROPERTY()
	float MoveElapsed = 0.0f;

};

USTRUCT(meta = (DisplayName = "Orbit Target"))
struct FIEAPORTFOLIO_API FSTT_OrbitTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_OrbitTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

protected:
	//Takes instance data since the node is stateless, everything is in structs
	bool PickOrbitPoint(FInstanceDataType& Data) const;
};