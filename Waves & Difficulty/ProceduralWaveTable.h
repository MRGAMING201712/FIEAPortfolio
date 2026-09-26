// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveSpawnerConfig.h"
#include "ProceduralWaveTable.generated.h"

/**
 * 
 */



USTRUCT(BlueprintType)
struct  FProceduralSpawnOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Waves")
	TSubclassOf<AActor> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Waves")
	float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Waves")
	int32 MinWaveToUnlock;
};


UCLASS()
class FIEAPORTFOLIO_API UProceduralWaveTable : public UDataAsset
{
	GENERATED_BODY()
	
private:

protected:

public:

	TSubclassOf<AActor> PickWeightedEnemy(int32 WaveIndex);
	FWaveDefinition GenerateProceduralWave(int32 WaveIndex, int32 WritenWaveCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Waves")
	TArray<FProceduralSpawnOption> SpawnOptions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Enemy Count")
	int32 BaseEnemyCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Enemy Count")
	int32 EnemyCountPerWave;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Enemy Count")
	int32 MaxEnemyCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Spawn Interval")
	float MinSpawnInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Spawn Interval")
	float MaxSpawnInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Procedural Tuning|Spawn Interval")
	float PerWaveSpawnInterval;
};
