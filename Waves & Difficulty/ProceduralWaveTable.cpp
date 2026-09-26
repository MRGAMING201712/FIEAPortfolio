// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralWaveTable.h"

struct FWaveDefinition;


TSubclassOf<AActor> UProceduralWaveTable::PickWeightedEnemy(int32 WaveIndex)
{
	float TotalWeight = 0.0f;
	TArray<FProceduralSpawnOption> FilteredList;

	for (const FProceduralSpawnOption& Option : SpawnOptions)
	{
		if (Option.MinWaveToUnlock <= WaveIndex)
		{
			TotalWeight += Option.Weight;
			FilteredList.Add(Option);
		}
	}

	if (FilteredList.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Filtered list empty. Falling back to unfiltered table."));
		FilteredList = SpawnOptions;

		for (const FProceduralSpawnOption& Option : FilteredList)
		{
			TotalWeight += Option.Weight;
		}
	}

	if (TotalWeight <= 0.0f || FilteredList.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Total weight is 0. Returning nullptr fallback."));
		return nullptr;
	}

	float RandomValue = FMath::RandRange(0.0f, TotalWeight);

	for (const FProceduralSpawnOption& Option : FilteredList)
	{
		RandomValue -= Option.Weight;

		if (RandomValue <= 0.0f)
		{
			return Option.EnemyClass;
		}
	}
	return FilteredList.Last().EnemyClass;
}

FWaveDefinition UProceduralWaveTable::GenerateProceduralWave(int32 WaveIndex, int32 WritenWaveCount)
{

	FWaveDefinition ProceduralWave;
	int32 WaveEnemyCount = FMath::Min(BaseEnemyCount + EnemyCountPerWave * (WaveIndex - WritenWaveCount), MaxEnemyCount);
	float EnemySpawnDelay = FMath::RandRange(MinSpawnInterval, MaxSpawnInterval);

	for (int32 i = 0; i < WaveEnemyCount; ++i)
	{
		TSubclassOf<AActor> ChosenEnemy = PickWeightedEnemy(WaveIndex);

		if (ChosenEnemy)
		{
			FSpawnEntry NewEntry;
			NewEntry.ActorClass = ChosenEnemy;
			NewEntry.SpawnCount = 1;
			NewEntry.SpawnDelay = EnemySpawnDelay;

			ProceduralWave.SpawnEntries.Add(NewEntry);
		}
	}

	return ProceduralWave;
}

