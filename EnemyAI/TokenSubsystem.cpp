#include "TokenSubsystem.h"
#include "CharacterBase.h"
#include "ActionStateComponent.h"

void UTokenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//Seed both pools
	FTokenPool MeleePool;
	MeleePool.MaxTokens = 20;
	Pools.Add(ETokenType::Melee, MeleePool);

	FTokenPool RangedPool;
	RangedPool.MaxTokens = 3;
	Pools.Add(ETokenType::Ranged, RangedPool);
}

void UTokenSubsystem::PrunePool(FTokenPool& Pool)
{
	for (auto It = Pool.Holders.CreateIterator(); It; ++It) //Safe way to iterate through a tset
	{
		if (!IsValid(It->Get()))
		{
			It.RemoveCurrent();
		}
	}
}

bool UTokenSubsystem::RequestToken(AActor* Requester, ETokenType TokenType)
{
	if (!Requester)
	{
		return false;
	}

	FTokenPool* Pool = Pools.Find(TokenType);
	if (!Pool)
	{
		return false;
	}

	PrunePool(*Pool);

	if (Pool->Holders.Contains(Requester))
	{
		return true;   //Already holding token
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(Requester))
	{
		if (UActionStateComponent* State = Character->GetActionState())
		{
			if (State->GetCurrentState() == ECharacterActionState::Dead)
			{
				Pool->Holders.Remove(Requester);
				return false;
			}
		}
	}

	if (Pool->Holders.Num() >= Pool->MaxTokens)
	{
		return false;
	}

	Pool->Holders.Add(Requester);
	return true;
}

void UTokenSubsystem::ReleaseToken(AActor* Requester, ETokenType TokenType)
{
	if (FTokenPool* Pool = Pools.Find(TokenType))
	{
		Pool->Holders.Remove(Requester);
	}
}

void UTokenSubsystem::ReleaseAllTokens(AActor* Requester)
{
	for (TPair<ETokenType, FTokenPool>& Pair : Pools)
	{
		Pair.Value.Holders.Remove(Requester);
	}
}

bool UTokenSubsystem::HasToken(AActor* Requester, ETokenType TokenType) const
{
	if (!Requester)
	{
		return false;
	}

	const FTokenPool* Pool = Pools.Find(TokenType);
	return Pool && Pool->Holders.Contains(Requester);
}

bool UTokenSubsystem::HasFreeToken(ETokenType TokenType) const
{
	const FTokenPool* Pool = Pools.Find(TokenType);
	return Pool && Pool->Holders.Num() < Pool->MaxTokens;
}

void UTokenSubsystem::SetMaxTokens(ETokenType TokenType, int32 NewMax)
{
	if (FTokenPool* Pool = Pools.Find(TokenType))
	{
		Pool->MaxTokens = FMath::Max(NewMax, 0);
	}
}