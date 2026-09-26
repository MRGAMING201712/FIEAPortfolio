// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaveSpawnerConfig.h"
#include "Engine/TargetPoint.h"
#include "ProceduralWaveTable.h"
#include "WaveSpawnerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCompleted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyKilled, float, XPReward, float, SoulsReward);



class UDifficultyProfile; 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UWaveSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	FTimerHandle TimerHandle_SpawnSequence;
	int32 CurrentWaveIndex;
	int32 CurrentEntryIndex;
	int32 SpawnsRemaining;
	int32 EnemiesAliveCount;
	bool bWaveSpawningComplete = false;
	int32 InitialWrittenWaveCount = 0;
	TArray<FWaveDefinition> RuntimeWaves;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSpawnedEnemyEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	//Indexing enemies so enemies can orbit evenly
	int32 SpawnIndexThisWave = 0;
	int32 TotalSpawnsThisWave = 0;

	int32 CountTotalSpawnsInWave(const FWaveDefinition& Wave) const;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Sets default values for this component's properties
	UWaveSpawnerComponent();

	UPROPERTY(BlueprintAssignable, Category = "Wave Events")
	FOnWaveStarted OnWaveStarted;
	UPROPERTY(BlueprintAssignable, Category = "Wave Events")
	FOnWaveCompleted OnWaveCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Wave Events")
	FOnAllWavesCompleted OnAllWavesCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Wave Events")
	FOnEnemyKilled OnEnemyKilled;

	

	void StartSpawnSequence();
	void StartCurrentWave();
	void StartCurrentEntry();	
	void SpawnNextActor();
	void AdvanceToNextEntry();
	void AdvanceToNextWave();

	void CheckWaveCompletion();


	UFUNCTION()
	void HandleEnemyDied(float XPReward, float SoulsReward);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	UWaveSpawnerConfig* WaveConfig;

	UPROPERTY(EditAnywhere, Category = "Wave Config")
	UDifficultyProfile* DifficultyProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	TArray<ATargetPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	bool bEndlessModeEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config", meta = (EditCondition = "bEndlessModeEnabled"))
	UProceduralWaveTable* ProceduralTableAsset;

};
