// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveSpawnerConfig.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config", meta = (ClampMin = "1"))
	int32 SpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config", meta = (ClampMin = "0.0"))
	float SpawnDelay = 1.0;
};
	
USTRUCT(BlueprintType)
struct FWaveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	TArray<FSpawnEntry> SpawnEntries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config", meta = (ClampMin = "0.0"))
	float DelayBeforeNextWave = 5.0;
};


UCLASS()
class FIEAPORTFOLIO_API UWaveSpawnerConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Config")
	TArray<FWaveDefinition> Waves;
	
};
