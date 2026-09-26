// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelingComponent.h"

// Sets default values for this component's properties
ULevelingComponent::ULevelingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULevelingComponent::OnAttributeChanged(FGameplayTag AttributeTag, float OldValue, float NewValue)
{
	if (AttributeTag != XPTag || bProcessingLevelUp || !XPToNextLevelCurve || !AttributeSet)
	{
		return;
	}

	bProcessingLevelUp = true;

	int32 LevelsGained = 0;

	while (true)
	{
		const float CurrentXP = AttributeSet->GetAttributeValue(XPTag);
		const float CurrentLevel = AttributeSet->GetAttributeValue(CurrentLevelTag);
		const float XPThreshold = XPToNextLevelCurve->GetFloatValue(CurrentLevel);
		const float LevelUpPoints = AttributeSet->GetAttributeValue(LevelUpPointsTag);

		if (XPThreshold <= 0.0f || CurrentXP < XPThreshold)
		{
			break;
		}

		AttributeSet->SetBaseValue(XPTag, CurrentXP - XPThreshold, ERecalcContext::LevelUp);
		AttributeSet->SetBaseValue(LevelUpPointsTag, LevelUpPoints + PointsPerLevelUp, ERecalcContext::LevelUp);
		AttributeSet->SetBaseValue(CurrentLevelTag, CurrentLevel + 1, ERecalcContext::LevelUp);
		++LevelsGained;
	}

	bProcessingLevelUp = false;

	if (LevelsGained > 0)
	{
		OnLevelGained.Broadcast(LevelsGained); //How many levels you gained when you earned xp in one go
	}
}

bool ULevelingComponent::TryLevelUpAttribute(FGameplayTag AttributeTag)
{
	if (!AttributeSet)
	{
		return false;
	}

	if (!LevelableAttributes.Contains(AttributeTag))
	{
		return false;
	}

	float LevelUpPoints = AttributeSet->GetAttributeValue(LevelUpPointsTag);
	if (LevelUpPoints < 1.0f)
	{
		return false;
	}

	float BaseValue = AttributeSet->GetAttributeValue(AttributeTag);
	if (BaseValue + 1 > AttributeSet->Definitions.FindRef(AttributeTag).MaxValue)
	{
		return false;
	}

	AttributeSet->SetBaseValue(LevelUpPointsTag, LevelUpPoints - 1, ERecalcContext::LevelUp);
	AttributeSet->SetBaseValue(AttributeTag, BaseValue + 1, ERecalcContext::LevelUp);

	OnAttributeLeveled.Broadcast(AttributeTag, BaseValue + 1);// New Level of attribute

	return true;
}

bool ULevelingComponent::CanLevelUpAttribute(FGameplayTag AttributeTag) const
{
	if (!AttributeSet)
	{
		return false;
	}

	if (!LevelableAttributes.Contains(AttributeTag))
	{
		return false;
	}

	float LevelUpPoints = AttributeSet->GetAttributeValue(LevelUpPointsTag);
	if (LevelUpPoints < 1.0f)
	{
		return false;
	}

	float BaseValue = AttributeSet->GetAttributeValue(AttributeTag);
	if (BaseValue + 1 > AttributeSet->Definitions.FindRef(AttributeTag).MaxValue)
	{
		return false;
	}

	return true;
}

void ULevelingComponent::GetXPProgress(float& OutCurrent, float& OutRequired) const
{
	if (!AttributeSet || !XPToNextLevelCurve)
	{
		return;
	}
	OutCurrent = 0;
	OutRequired = 0;

	const float CurrentLevel = AttributeSet->GetAttributeValue(CurrentLevelTag);
	const float CurrentXP = AttributeSet->GetAttributeValue(XPTag);
	OutCurrent = CurrentXP;
	OutRequired = XPToNextLevelCurve->GetFloatValue(CurrentLevel);

}

// Called when the game starts
void ULevelingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AttributeSet = GetOwner()->FindComponentByClass<UAttributeSet>();
	if (AttributeSet)
	{
		AttributeSet->OnAttributeChanged.AddDynamic(this, &ULevelingComponent::OnAttributeChanged);
	}
}


// Called every frame
void ULevelingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

 