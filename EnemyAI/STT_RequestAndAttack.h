#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "TokenSubsystem.h"
#include "STT_RequestAndAttack.generated.h"

class ACharacterBase;

UENUM()
enum class EAIActionType : uint8
{
	LightAttack,
	HeavyAttack,
	Ability
};

USTRUCT()
struct FSTT_RequestAndAttackInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EAIActionType ActionType = EAIActionType::LightAttack;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 AbilitySlot = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	ETokenType TokenType;

	//Runtime stuff, tracked while running
	UPROPERTY()
	bool bHoldsToken = false;
};

USTRUCT(meta = (DisplayName = "Request And Attack"))
struct FIEAPORTFOLIO_API FSTT_RequestAndAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_RequestAndAttackInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};