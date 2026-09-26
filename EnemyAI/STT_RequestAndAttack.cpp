#include "STT_RequestAndAttack.h"
#include "StateTreeExecutionContext.h"
#include "CharacterBase.h"
#include "ComboComponent.h"
#include "TokenSubsystem.h"

EStateTreeRunStatus FSTT_RequestAndAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.bHoldsToken = false;

	ACharacterBase* Character = Cast<ACharacterBase>(Data.Actor);
	if (!Character)
	{
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = Character->GetWorld();
	UTokenSubsystem* Tokens = World ? World->GetSubsystem<UTokenSubsystem>() : nullptr;
	if (!Tokens)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!Tokens->RequestToken(Character, Data.TokenType))
	{
		return EStateTreeRunStatus::Failed;   //No tokens available to attack
	}

	Data.bHoldsToken = true;

	//Branch for abilities or attacks

	bool bStarted = false;

	switch (Data.ActionType)
	{
	case EAIActionType::LightAttack:
		bStarted = Character->TryAttack(false);
		break;

	case EAIActionType::HeavyAttack:
		bStarted = Character->TryAttack(true);
		break;

	case EAIActionType::Ability:
		bStarted = Character->TryCastAbility(Data.AbilitySlot);
		break;
	}

	if (!bStarted)
	{
		return EStateTreeRunStatus::Failed;   //ExitState still exits
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_RequestAndAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& Data = Context.GetInstanceData(*this);

	ACharacterBase* Character = Cast<ACharacterBase>(Data.Actor);
	if (!Character)
	{
		return EStateTreeRunStatus::Failed;
	}

	UActionStateComponent* State = Character->GetActionState();
	if (!State)
	{
		return EStateTreeRunStatus::Failed;
	}

	//Action finished
	if (State->GetCurrentState() == ECharacterActionState::None)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_RequestAndAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);

	if (!Data.bHoldsToken)
	{
		return;
	}

	if (Data.Actor)
	{
		if (UWorld* World = Data.Actor->GetWorld())
		{
			if (UTokenSubsystem* Tokens = World->GetSubsystem<UTokenSubsystem>())
			{
				Tokens->ReleaseToken(Data.Actor, Data.TokenType);
			}
		}
	}

	Data.bHoldsToken = false;
}