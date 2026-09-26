// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "AttributeSetDefinition.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"

UAttributeSet::UAttributeSet()
{

	PrimaryComponentTick.bCanEverTick = false;
}


//  Reads

float UAttributeSet::GetAttributeValue(FGameplayTag AttributeTag) const
{
	if (const FAttributeInstance* Inst = Attributes.Find(AttributeTag))
	{
		const FAttributeData* Def = Definitions.Find(AttributeTag);

		if (!Def->bIsResource)
		{
			return GetResolvedValue(AttributeTag);
		}

		else
		{
			return Inst->CurrentValue;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GetAttributeValue: Attribute [%s] not found on %s"), *AttributeTag.ToString(), GetOwner() ? *GetOwner()->GetName() : TEXT("<no owner>"));
	return 0.0f;
}

float UAttributeSet::GetResolvedValue(FGameplayTag AttributeTag) const
{
	const FAttributeInstance* Inst = Attributes.Find(AttributeTag);
	if (!Inst)
	{
		return 0.0f;
	}

	float AdditiveSum = 0.0f;
	float MultiplicativeSum = 0.0f;
	bool  bHasOverride = false;
	float OverrideValue = 0.0f;
	float Value;

	if (const FAttributeModifierArray* Wrapper = ActiveModifiers.Find(AttributeTag))
	{
		for (const FAttributeModifier& Mod : Wrapper->Modifiers)
		{
			switch (Mod.Operation)
			{
			case EModifierOperation::Additive:
				AdditiveSum += Mod.Magnitude;
				break;

			case EModifierOperation::Multiplicative:
				MultiplicativeSum += Mod.Magnitude;
				break;

			case EModifierOperation::Override:
				// Last override applied wins.
				bHasOverride = true;
				OverrideValue = Mod.Magnitude;
				break;
			}
		}
	}

	// ResolvedBase, not BaseValue. For derived attributes BaseValue is meaningless.
	if (bHasOverride)
	{
		Value = OverrideValue;
	}
	else
	{
		Value = (Inst->ResolvedBase + AdditiveSum)* (1.0f + MultiplicativeSum);
	}


	if (const FAttributeData* Def = Definitions.Find(AttributeTag))
	{
		Value = FMath::Clamp(Value, Def->MinValue, Def->MaxValue);
	}

	return Value;
}

void UAttributeSet::GetResourceValues(FGameplayTag AttributeTag, float& OutCurrent, float& OutMax) const
{
	OutCurrent = 0.0f;
	OutMax = 0.0f;

	if (const FAttributeInstance* Inst = Attributes.Find(AttributeTag))
	{
		OutCurrent = Inst->CurrentValue;
		OutMax = GetResolvedValue(AttributeTag);
	}
}


//  Modifiers

FGuid UAttributeSet::ApplyModifier(FGameplayTag TargetAttribute, EModifierOperation Operation, float Magnitude, float Duration, UObject* Source, ERecalcContext Context, FGameplayTagContainer BlockedByTags)
{
	if (!Attributes.Contains(TargetAttribute))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyModifier: Attribute [%s] does not exist"), *TargetAttribute.ToString());
		return FGuid();
	}

	if (!CanApplyChange(BlockedByTags))
	{
		return FGuid();
	}

	FAttributeModifier NewMod;
	NewMod.TargetAttribute = TargetAttribute;
	NewMod.Operation = Operation;
	NewMod.Magnitude = Magnitude;
	NewMod.Duration = Duration;
	NewMod.Source = Source;
	NewMod.Handle = FGuid::NewGuid();
	NewMod.BlockedByTags = BlockedByTags;


	const float OldResolved = GetResolvedValue(TargetAttribute);
	ActiveModifiers.FindOrAdd(TargetAttribute).Modifiers.Add(NewMod);
	RecalculateAttribute(TargetAttribute, Context, 0, OldResolved);

	if (NewMod.Duration > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("ExpireModifier"), TargetAttribute, NewMod.Handle);

			World->GetTimerManager().SetTimer(TimerHandle, TimerDel, NewMod.Duration, false);
			ActiveTimers.Add(NewMod.Handle, TimerHandle);
		}
	}

	return NewMod.Handle;
}

bool UAttributeSet::RemoveModifier(FGameplayTag TargetAttribute, FGuid ModifierHandle, ERecalcContext Context)
{
	FAttributeModifierArray* Wrapper = ActiveModifiers.Find(TargetAttribute);
	if (!Wrapper)
	{
		return false;
	}

	const int32 RemovedCount = Wrapper->Modifiers.RemoveAll([ModifierHandle](const FAttributeModifier& Mod)
	{
		return Mod.Handle == ModifierHandle;
	});

	if (RemovedCount == 0)
	{
		return false;
	}

	if (FTimerHandle* FoundTimer = ActiveTimers.Find(ModifierHandle))
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(*FoundTimer);
		}
		ActiveTimers.Remove(ModifierHandle);
	}

	RecalculateAttribute(TargetAttribute, Context, 0);
	return true;
}

void UAttributeSet::ExpireModifier(FGameplayTag TargetAttribute, FGuid ModifierHandle)
{
	RemoveModifier(TargetAttribute, ModifierHandle, ERecalcContext::Buff);
}


//  Writes

bool UAttributeSet::ApplyInstantChange(const FInstantChange& Struct)

{
	FAttributeInstance* Inst = Attributes.Find(Struct.AttributeTag);
	const FAttributeData* Def = Definitions.Find(Struct.AttributeTag);

	if (!Inst || !Def)
	{
		return false;
	}

	if (!CanApplyChange(Struct.BlockedByTags))
	{
		return false;
	}

	if (!Def->bIsResource)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyInstantChange on non-resource [%s] - use SetBaseValue"), *Struct.AttributeTag.ToString());
		return false;
	}

	const float MaxVal = GetResolvedValue(Struct.AttributeTag);
	const float OldValue = Inst->CurrentValue;

	Inst->CurrentValue = FMath::Clamp(OldValue + Struct.Delta, 0.0f, MaxVal);

	if (!FMath::IsNearlyEqual(OldValue, Inst->CurrentValue))
	{
		if (Struct.Delta < 0.0f)
		{
			LastDecreaseTimestamp.Add(Struct.AttributeTag, GetWorld()->GetTimeSeconds());
		}
		OnResourceChanged.Broadcast(Struct.AttributeTag, Inst->CurrentValue, MaxVal);
		CheckDepletionEdge(Struct.AttributeTag);
	}

	return true;
}

void UAttributeSet::SetBaseValue(FGameplayTag AttributeTag, float NewBase, ERecalcContext Context)
{
	FAttributeInstance* Inst = Attributes.Find(AttributeTag);
	if (!Inst)
	{
		return;
	}

	Inst->BaseValue = NewBase;
	RecalculateAttribute(AttributeTag, Context, 0);
}

void UAttributeSet::RegenTick()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	for (const auto& Pair : Definitions)
	{
		const FGameplayTag& Tag = Pair.Key;
		const FAttributeData& Def = Pair.Value;

		if (Def.bRegenerates)
		{
			float LastDecrease = LastDecreaseTimestamp.Contains(Tag) ? LastDecreaseTimestamp[Tag] : 0.0f;

			if ((CurrentTime - LastDecrease) >= Def.RegenDelay)
			{
				if (FAttributeInstance* Inst = Attributes.Find(Tag))
				{
					float ResolvedMax = GetResolvedValue(Tag);

					float RegenAmount = Def.RegenRate * RegenTickInterval;

					float OldValue = Inst->CurrentValue;
					Inst->CurrentValue = FMath::Clamp(Inst->CurrentValue + RegenAmount, 0.0f, ResolvedMax);

					if (Inst->CurrentValue > OldValue && OnResourceChanged.IsBound())
					{
						OnResourceChanged.Broadcast(Tag, Inst->CurrentValue, ResolvedMax);
						CheckDepletionEdge(Tag);
					}
				}
			}
		}
	}
}

void UAttributeSet::AddImmunityTag(FGameplayTag AttributeTag)
{
	if (AttributeTag.IsValid())
	{
		ActiveImmunityTags.AddTag(AttributeTag);
	}
}

void UAttributeSet::RemoveImmunityTag(FGameplayTag AttributeTag)
{
	if (AttributeTag.IsValid())
	{
		ActiveImmunityTags.RemoveTag(AttributeTag);
	}
}
//  Recalculation

void UAttributeSet::RecalculateAttribute(FGameplayTag AttributeTag, ERecalcContext Context, int32 Depth, float KnownOldResolved)
{
	if (Depth > MaxDerivationDepth)
	{
		UE_LOG(LogTemp, Error, TEXT("RecalculateAttribute: depth limit hit at [%s] - check for a circular derivation"), *AttributeTag.ToString());
		return;
	}

	FAttributeInstance* Inst = Attributes.Find(AttributeTag);
	const FAttributeData* Def = Definitions.Find(AttributeTag);

	if (!Inst || !Def)
	{
		return;
	}

	// need old max
	const float OldResolved = (KnownOldResolved >= 0.0f) ? KnownOldResolved : GetResolvedValue(AttributeTag);
	if (Def->bIsDerived && Def->DerivationCurve)
	{
		const float SourceValue = GetResolvedValue(Def->SourceTag);
		Inst->ResolvedBase = Def->DerivationCurve->GetFloatValue(SourceValue);
	}
	else
	{
		Inst->ResolvedBase = Inst->BaseValue;
	}

	const float NewResolved = GetResolvedValue(AttributeTag);

	
	if (Def->bIsResource)
	{
		if (!FMath::IsNearlyEqual(OldResolved, NewResolved))
		{
			ApplyMaxChangePolicy(AttributeTag, OldResolved, NewResolved, Context);
		}
	}
	else
	{
		const float OldCurrent = Inst->CurrentValue;
		Inst->CurrentValue = NewResolved;

		if (!FMath::IsNearlyEqual(OldCurrent, NewResolved))
		{
			OnAttributeChanged.Broadcast(AttributeTag, OldCurrent, NewResolved);
		}
	}

	//copy list before recursion
	if (const TArray<FGameplayTag>* Deps = DependencyMap.Find(AttributeTag))
	{
		const TArray<FGameplayTag> DepsCopy = *Deps;
		for (const FGameplayTag& Dependent : DepsCopy)
		{
			RecalculateAttribute(Dependent, Context, Depth + 1);
		}
	}
}

void UAttributeSet::ApplyMaxChangePolicy(FGameplayTag AttributeTag, float OldMax, float NewMax, ERecalcContext Context)
{
	FAttributeInstance* Inst = Attributes.Find(AttributeTag);
	if (!Inst)
	{
		return;
	}

	EMaxChangePolicy Policy = EMaxChangePolicy::PreserveAbsolute;
	switch (Context)
	{
	case ERecalcContext::LevelUp:
		// PreserveRatio: Bar grows with you 5/10 -> 10/20
		Policy = EMaxChangePolicy::FillToMax;
		break;

	case ERecalcContext::Respec:
	case ERecalcContext::Init:
		Policy = EMaxChangePolicy::FillToMax;
		break;

	case ERecalcContext::Equipment:
	case ERecalcContext::Buff:
	default:
		//  Prevents healing from equiping items repeatedly
		Policy = EMaxChangePolicy::PreserveAbsolute;
		break;
	}

	const float OldCurrent = Inst->CurrentValue;

	switch (Policy)
	{
	case EMaxChangePolicy::PreserveRatio:
	{
		float Ratio = 1.0f;

		if ((OldMax > KINDA_SMALL_NUMBER))
		{
			Ratio = (OldCurrent / OldMax);
		}

		else
		{
			Ratio = 1.0f;
		}
	

		Inst->CurrentValue = NewMax * Ratio;
		break;
	}

	case EMaxChangePolicy::PreserveAbsolute:
		Inst->CurrentValue = FMath::Min(OldCurrent, NewMax);
		break;

	case EMaxChangePolicy::FillToMax:
		Inst->CurrentValue = NewMax;
		break;
	}

	Inst->CurrentValue = FMath::Clamp(Inst->CurrentValue, 0.0f, NewMax);

	if (!FMath::IsNearlyEqual(OldCurrent, Inst->CurrentValue))
	{
		OnResourceChanged.Broadcast(AttributeTag, Inst->CurrentValue, NewMax);
		CheckDepletionEdge(AttributeTag);
	}
}


//  Dependency

void UAttributeSet::BuildDependencyMap()
{
	DependencyMap.Empty();

	for (const TPair<FGameplayTag, FAttributeData>& Pair : Definitions)
	{
		const FGameplayTag& Tag = Pair.Key;   // the key is the identity
		const FAttributeData& Row = Pair.Value;

		if (!Row.bIsDerived || !Row.SourceTag.IsValid())
		{
			continue;
		}

		DependencyMap.FindOrAdd(Row.SourceTag).AddUnique(Tag);
	}
}

bool UAttributeSet::ValidateNoCycles() const
{
	bool bAllValid = true;

	for (const TPair<FGameplayTag, FAttributeData>& Pair : Definitions)
	{
		if (!Pair.Value.bIsDerived)
		{
			continue;
		}

		TArray<FGameplayTag> Visited;
		Visited.Add(Pair.Key);

		FGameplayTag Cursor = Pair.Value.SourceTag;

		while (Cursor.IsValid())
		{
			if (Visited.Contains(Cursor))
			{
				UE_LOG(LogTemp, Error, TEXT("Circular derivation: [%s] chain revisits [%s]"), *Pair.Key.ToString(), *Cursor.ToString());
				bAllValid = false;
				break;
			}

			Visited.Add(Cursor);

			const FAttributeData* CursorDef = Definitions.Find(Cursor);
			if (!CursorDef || !CursorDef->bIsDerived)
			{
				break;   // reached a root
			}

			Cursor = CursorDef->SourceTag;
		}
	}

	return bAllValid;
}

// Delegate Help

void UAttributeSet::CheckDepletionEdge(FGameplayTag AttributeTag)
{
	bool bIsAttributeDepleted = Attributes[AttributeTag].bWasDepleted;

	if (Attributes[AttributeTag].CurrentValue <= 0 && !bIsAttributeDepleted)
	{
		Attributes[AttributeTag].bWasDepleted = true;
		if (OnAttributeDepleted.IsBound())
		{
			OnAttributeDepleted.Broadcast(AttributeTag);
		}
	}

	else if (Attributes[AttributeTag].CurrentValue > 0 && bIsAttributeDepleted)
	{
		Attributes[AttributeTag].bWasDepleted = false;
		if (OnAttributeRestoredFromZero.IsBound())
		{
			OnAttributeRestoredFromZero.Broadcast(AttributeTag);
		}
	}
}

bool UAttributeSet::CanApplyChange(FGameplayTagContainer BlockedTags) const
{
	return !BlockedTags.HasAny(ActiveImmunityTags);
}


//Begin/End Play
void UAttributeSet::BeginPlay()
{
	Super::BeginPlay();

	if (!DefinitionTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAttributeSet on %s has no DefinitionTable assigned"), GetOwner() ? *GetOwner()->GetName() : TEXT("<no owner>"));
		return;
	}

	//Cache definitions 
	Definitions = DefinitionTable->Attributes;

	for (const TPair<FGameplayTag, FAttributeData>& Pair : Definitions)
	{
		FAttributeInstance NewInstance;
		NewInstance.BaseValue = Pair.Value.BaseValue;
		NewInstance.ResolvedBase = Pair.Value.BaseValue;
		NewInstance.CurrentValue = Pair.Value.BaseValue;

		Attributes.Add(Pair.Key, NewInstance);
	}

	//dependency graph, then validate it.
	BuildDependencyMap();
	ValidateNoCycles();

	//Resolve roots only, derivated attributes will be reached through recursion
	for (const TPair<FGameplayTag, FAttributeData>& Pair : Definitions)
	{
		if (!Pair.Value.bIsDerived)
		{
			RecalculateAttribute(Pair.Key, ERecalcContext::Init, 0);
		}
	}

	for (const TPair<FGameplayTag, FAttributeData>& Pair : Definitions)
	{
		if (Pair.Value.bIsResource)
		{
			const float Resolved = GetResolvedValue(Pair.Key);
			UE_LOG(LogTemp, Warning, TEXT("Fill %s -> %f (mods: %d)"),
				*Pair.Key.ToString(),
				Resolved,
				ActiveModifiers.Contains(Pair.Key) ? ActiveModifiers[Pair.Key].Modifiers.Num() : 0);
			Attributes[Pair.Key].CurrentValue = Resolved;
		}
	}

	//Master Regen Timer
	GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UAttributeSet::RegenTick, RegenTickInterval, true);
}

void UAttributeSet::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
	ActiveTimers.Empty();

	Super::EndPlay(EndPlayReason);
}