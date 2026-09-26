// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawnerComponent.h"
#include "NavigationSystem.h"
#include "DifficultyScalable.h"
#include "DifficultyManager.h"
#include "DifficultyProfile.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"   
#include "WaveGamemode.h"


// Sets default values for this component's properties
UWaveSpawnerComponent::UWaveSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}



void UWaveSpawnerComponent::StartSpawnSequence()
{
	if (!WaveConfig || WaveConfig->Waves.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveConfig is null or empty on %s"), *GetOwner()->GetName());
		return;
	}

	CurrentWaveIndex = 0;
	StartCurrentWave();
}

void UWaveSpawnerComponent::StartCurrentWave()
{
	if (OnWaveStarted.IsBound())
	{
		OnWaveStarted.Broadcast(CurrentWaveIndex);
	}

	CurrentEntryIndex = 0;
	bWaveSpawningComplete = false;
	EnemiesAliveCount = 0;

	StartCurrentEntry();
}

void UWaveSpawnerComponent::StartCurrentEntry()
{
	const FWaveDefinition& CurrentWave = RuntimeWaves[CurrentWaveIndex];
	if (CurrentEntryIndex >= CurrentWave.SpawnEntries.Num())
	{
		bWaveSpawningComplete = true;
		CheckWaveCompletion();
		return;
	}

	const FSpawnEntry& CurrentEntry = CurrentWave.SpawnEntries[CurrentEntryIndex];
	SpawnsRemaining = CurrentEntry.SpawnCount;

	if (SpawnsRemaining <= 0)
	{
		AdvanceToNextEntry();
		return;
	}
	SpawnIndexThisWave = 0;
	TotalSpawnsThisWave = CountTotalSpawnsInWave(CurrentWave);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnSequence, this, &UWaveSpawnerComponent::SpawnNextActor, CurrentEntry.SpawnDelay, true);
}

void UWaveSpawnerComponent::SpawnNextActor()
{
	const FWaveDefinition& CurrentWave = RuntimeWaves[CurrentWaveIndex];
	const FSpawnEntry& CurrentEntry = CurrentWave.SpawnEntries[CurrentEntryIndex];

	FVector SpawnLocation = GetOwner()->GetActorLocation();
	FRotator SpawnRotation = GetOwner()->GetActorRotation();

	if (!SpawnPoints.IsEmpty())
	{
		int32 RandomInt = FMath::RandHelper(SpawnPoints.Num());
		SpawnLocation = SpawnPoints[RandomInt]->GetActorLocation();
		SpawnRotation = SpawnPoints[RandomInt]->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Spawning at Target Point Index [%d]. Location: %s"), RandomInt, *SpawnLocation.ToString());
	}

	else
	{
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

		if (NavSystem)
		{
			FNavLocation RandomNavLocation;
			bool bFoundPoint = NavSystem->GetRandomPointInNavigableRadius(GetOwner()->GetActorLocation(), 2000.0f, RandomNavLocation);

			if (bFoundPoint)
			{
				SpawnLocation = RandomNavLocation.Location;

				if (CurrentEntry.ActorClass)
				{
					//Get the Default Object of the class to read its capsule size before spawning
					if (ACharacter* DefaultEnemy = Cast<ACharacter>(CurrentEntry.ActorClass->GetDefaultObject()))
					{
						SpawnLocation.Z += DefaultEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
						UE_LOG(LogTemp, Warning, TEXT("Spawning at NavMesh Location: %s"), *SpawnLocation.ToString());
					}
				}
			}
		}
		else
		{ 		
			UE_LOG(LogTemp, Warning, TEXT("Failed to find a NavMesh point. Falling back to origin."));
		}
	}

	if (CurrentEntry.ActorClass)
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(CurrentEntry.ActorClass, SpawnLocation, SpawnRotation);
		
		if (SpawnedActor)
		{
			EnemiesAliveCount++;
			SpawnedActor->OnEndPlay.AddDynamic(this, &UWaveSpawnerComponent::OnSpawnedEnemyEndPlay);
		}

		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(SpawnedActor))
		{
			Enemy->OnEnemyDeath.AddDynamic(this, &UWaveSpawnerComponent::HandleEnemyDied);
		}
		

		if (SpawnedActor && DifficultyProfile && CurrentWaveIndex >= InitialWrittenWaveCount)
		{
			UDifficultyManager* DiffSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDifficultyManager>();
			if (DiffSubsystem)
			{
				TMap<FGameplayTag, float> ResolvedModifiers = DiffSubsystem->GetFinalModifiers(DifficultyProfile, CurrentWaveIndex);

				if (SpawnedActor->Implements<UDifficultyScalable>())
				{
					IDifficultyScalable::Execute_ApplyDifficultyModifiers(SpawnedActor, ResolvedModifiers);
				}

				if (ACharacterBase* Character = Cast<ACharacterBase>(SpawnedActor))
				{
					const float Angle = (360.0f / FMath::Max(TotalSpawnsThisWave, 1)) * SpawnIndexThisWave;
					Character->SetOrbitAngleOffset(Angle);
				}

				++SpawnIndexThisWave;
			}
		}
	}

	SpawnsRemaining--;

	if (SpawnsRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnSequence);
		AdvanceToNextEntry();
	}
}


void UWaveSpawnerComponent::AdvanceToNextEntry()
{
	CurrentEntryIndex++;
	StartCurrentEntry();
}

void UWaveSpawnerComponent::AdvanceToNextWave()
{
	if (OnWaveCompleted.IsBound())
	{
		OnWaveCompleted.Broadcast(CurrentWaveIndex);
	}

	CurrentWaveIndex++;
	bWaveSpawningComplete = false;

	if (CurrentWaveIndex < RuntimeWaves.Num())
	{
		const FWaveDefinition& CurrentWave = RuntimeWaves[CurrentWaveIndex];
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimerHandle_SpawnSequence, this, &UWaveSpawnerComponent::StartCurrentWave, CurrentWave.DelayBeforeNextWave, false);
		}
	}

	else if (bEndlessModeEnabled && ProceduralTableAsset)
	{

		FWaveDefinition NewEndlessWave = ProceduralTableAsset->GenerateProceduralWave(CurrentWaveIndex, InitialWrittenWaveCount);

		RuntimeWaves.Add(NewEndlessWave);

		const FWaveDefinition& CurrentWave = RuntimeWaves[CurrentWaveIndex];
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimerHandle_SpawnSequence, this, &UWaveSpawnerComponent::StartCurrentWave, CurrentWave.DelayBeforeNextWave, false);
		}
	}

	else
	{
		if (OnAllWavesCompleted.IsBound())
		{
			OnAllWavesCompleted.Broadcast();
		}
	}
}

void UWaveSpawnerComponent::OnSpawnedEnemyEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy End Play FUnction"));
	EnemiesAliveCount = FMath::Max(EnemiesAliveCount - 1, 0);
	CheckWaveCompletion();
}

void UWaveSpawnerComponent::CheckWaveCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("Can I spawn?."));

	if (bWaveSpawningComplete && EnemiesAliveCount <= 0)
	{
		AdvanceToNextWave();
	}
}

void UWaveSpawnerComponent::HandleEnemyDied(float XPReward, float SoulsReward)
{
	OnEnemyKilled.Broadcast(XPReward, SoulsReward);
}



// Called when the game starts
void UWaveSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AWaveGamemode* GM = Cast<AWaveGamemode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->RegisterWaveSpawner(this);
	}

	if (WaveConfig)
	{
		RuntimeWaves = WaveConfig->Waves;
		InitialWrittenWaveCount = WaveConfig->Waves.Num();
	}

	StartSpawnSequence();
	
}


int32 UWaveSpawnerComponent::CountTotalSpawnsInWave(const FWaveDefinition& Wave) const
{
	int32 Total = 0;
	for (const FSpawnEntry& Entry : Wave.SpawnEntries)
	{
		Total += Entry.SpawnCount;
	}
	return Total;
}

// Called every frame
void UWaveSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

