#include "STC_HasAttackToken.h"
#include "StateTreeExecutionContext.h"
#include "TokenSubsystem.h"
#include "GameFramework/Actor.h"

bool FSTC_HasAttackToken::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& Data = Context.GetInstanceData(*this);

	if (!Data.Actor)
	{
		return false;
	}

	UWorld* World = Data.Actor->GetWorld();
	if (!World)
	{
		return false;
	}

	UTokenSubsystem* Tokens = World->GetSubsystem<UTokenSubsystem>();
	if (!Tokens)
	{
		return false;
	}

	return Tokens->HasToken(Data.Actor, Data.TokenType);
}