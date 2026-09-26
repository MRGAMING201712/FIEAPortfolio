// Fill out your copyright notice in the Description page of Project Settings.


#include "DifficultyManager.h"
#include "DifficultyProfile.h"
#include "Curves/CurveFloat.h"
#include "GameplayTagContainer.h"


TMap<FGameplayTag, float> UDifficultyManager::GetFinalModifiers(UDifficultyProfile* Profile, int32 WaveIndex)
{
	TMap<FGameplayTag, float> FinalModifiers;

	if (!Profile)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetFinalModifiers: Provided Profile is null."));
		return FinalModifiers;
	}

	float CurveScalar = 1.0;

	if (Profile->ProgressionCurve)
	{
		CurveScalar = Profile->ProgressionCurve->GetFloatValue(WaveIndex);
	}

	for (const TPair<FGameplayTag, float>& ModifierPair : Profile->BaseModifiers)
	{
		float ScaledValue = ModifierPair.Value * CurveScalar;
		FinalModifiers.Add(ModifierPair.Key, ScaledValue);
	}
	return FinalModifiers;
}

void UDifficultyManager::DebugTesting(UDifficultyProfile* Profile, int32 WaveIndex)
{
	if (!Profile)
	{
		UE_LOG(LogTemp, Error, TEXT("Debug Test Failed: No Difficulty Profile assigned."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("--- Testing Difficulty For Wave %d ---"), WaveIndex);

	// Get the math results
	TMap<FGameplayTag, float> ResultMap = GetFinalModifiers(Profile, WaveIndex);

	// Print the results to check the scaled values
	for (const TPair<FGameplayTag, float>& ResultPair : ResultMap)
	{
		UE_LOG(LogTemp, Log, TEXT("Modifier [%s] = %f"), *ResultPair.Key.ToString(), ResultPair.Value);
	}
}
