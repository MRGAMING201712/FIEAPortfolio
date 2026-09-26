// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponent.h"
#include "AttributeSet.h"
#include "CharacterBase.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool UAbilityComponent::TryActivateAbility(int32 Slot)
{
	float Unused = 0.0f;
	if (!CanActivate(Slot, Unused))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityFail_5"));

		bIsCasting = false;
		PendingSlot = -1;
		return false;
	}

	UAbilityDefinition* AbilityDef = EquippedAbilities[Slot];

	FInstantChange AttributeChangeInstance; 
	AttributeChangeInstance.AttributeTag = AbilityDef->CostAttributeTag;
	AttributeChangeInstance.Delta = AbilityDef->Cost;

	AttributeSet->ApplyInstantChange(AttributeChangeInstance);
	bIsCasting = true;
	PendingSlot = Slot;
	CooldownEndTimes.Add(AbilityDef, GetWorld()->GetTimeSeconds() + AbilityDef->Cooldown);

	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (!AbilityDef->CastMontage)
		{	
			UE_LOG(LogTemp, Warning, TEXT("AbilityFail_6"));

			bIsCasting = false;
			PendingSlot = -1;
			return false;
		}

		if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilityPlayMontage"));

			AnimInstance->Montage_Play(AbilityDef->CastMontage);
			ActiveCastMontage = AbilityDef->CastMontage;

			FOnMontageEnded EndedDelegate;
			EndedDelegate.BindUObject(this, &UAbilityComponent::OnCastMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndedDelegate, AbilityDef->CastMontage);
		}
	}
	return true;
}

void UAbilityComponent::CancelCast()
{
	if (ActiveCastMontage)
	{
		UAnimMontage* MontageToStop = ActiveCastMontage;
		ActiveCastMontage = nullptr;

		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				UE_LOG(LogTemp, Warning, TEXT("CancelCast_01"));
				AnimInstance->Montage_Stop(0.2f, MontageToStop);
			}
		}
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwner()))
	{
		if (UActionStateComponent* ActionState = Character->GetActionState())
		{
			if (ActionState->GetCurrentState() == ECharacterActionState::Casting)
			{
				UE_LOG(LogTemp, Warning, TEXT("CancelCast_02"));
				ActionState->ForceEnterState(ECharacterActionState::None);
			}
		}
	}

	bIsCasting = false;
	PendingSlot = -1;
}

void UAbilityComponent::OnCastMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	CancelCast();
}

void UAbilityComponent::HandleCastNotify()
{
	if (!EquippedAbilities.IsValidIndex(PendingSlot))
	{
		return;
	}

	UAbilityDefinition* AbilityDef = EquippedAbilities[PendingSlot];

	if (!AbilityDef || !AbilityDef->EffectClass)
	{
		return;
	}

	if (UAbilityEffect* Effect = AbilityDef->EffectClass->GetDefaultObject<UAbilityEffect>())
	{
		Effect->Execute(GetOwner(), AbilityDef);
		PendingSlot = -1;
	}
}

bool UAbilityComponent::CanActivate(int32 Slot, float& OutCooldownRemaining) const
{
	OutCooldownRemaining = 0.0f;

	if( Slot < 0 || Slot >= EquippedAbilities.Num()  || bIsCasting)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityFail_1"));

		return false;
	}

	const UAbilityDefinition* AbilityDef = EquippedAbilities[Slot];
	const UWorld* World = GetWorld();

	if (!AbilityDef || !World || !AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityFail_2"));

		return false;
	}

	const float Now = World->GetTimeSeconds();
	const float EndTime = CooldownEndTimes.FindRef(AbilityDef);
	if (Now < EndTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityFail_3"));

		OutCooldownRemaining = EndTime - Now;
		return false;
	}

	if (FMath::Abs(AbilityDef->Cost) > AttributeSet->GetAttributeValue(AbilityDef->CostAttributeTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityFail_4"));

		return false;
	}

	return true;
}

bool UAbilityComponent::IsCasting() const
{
	return bIsCasting;
}



// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet = GetOwner()->FindComponentByClass<UAttributeSet>();
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


