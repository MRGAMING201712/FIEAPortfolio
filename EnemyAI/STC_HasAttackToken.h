#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "TokenSubsystem.h"
#include "STC_HasAttackToken.generated.h"

USTRUCT()
struct FSTC_HasAttackTokenInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	ETokenType TokenType;
};

USTRUCT(meta = (DisplayName = "Has Attack Token"))
struct FIEAPORTFOLIO_API FSTC_HasAttackToken : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTC_HasAttackTokenInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};