// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStateComponent.h"

// Sets default values for this component's properties
UActionStateComponent::UActionStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool UActionStateComponent::CanEnterState(ECharacterActionState NewState) const
{
	if (CurrentState == ECharacterActionState::Dead)
	{
		return false;
	}

	switch (CurrentState)
	{
	case ECharacterActionState::None:
		return true;

	case ECharacterActionState::Attacking:
	case ECharacterActionState::Casting:
		return NewState == ECharacterActionState::Dodging
			|| NewState == ECharacterActionState::Staggered
			|| NewState == ECharacterActionState::Dead;

	case ECharacterActionState::Dodging:
	case ECharacterActionState::Staggered:
		return NewState == ECharacterActionState::Staggered
			|| NewState == ECharacterActionState::Dead;

	default:
		return false;
	}
}

bool UActionStateComponent::TryEnterState(ECharacterActionState NewState)
{
	if (CanEnterState(NewState))
	{
		ECharacterActionState OldState = CurrentState;
		CurrentState = NewState;
		OnActionStateChanged.Broadcast(OldState, NewState);
		return true;
	}

	return false;
}

void UActionStateComponent::ForceEnterState(ECharacterActionState NewState)
{
	if (NewState == CurrentState)
	{
		return;
	}

	ECharacterActionState OldState = CurrentState;
	CurrentState = NewState;
	OnActionStateChanged.Broadcast(OldState, NewState);
}

ECharacterActionState UActionStateComponent::GetCurrentState()
{
	return CurrentState;
}

// Called when the game starts
void UActionStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActionStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

