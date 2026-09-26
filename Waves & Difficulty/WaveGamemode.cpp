// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGamemode.h"
#include "Kismet/GameplayStatics.h"   

void AWaveGamemode::RegisterWaveSpawner(UWaveSpawnerComponent* Spawner)
{
	if (!Spawner)
	{
		return;
	}

	WaveSpawner = Spawner;
	Spawner->OnEnemyKilled.AddDynamic(this, &AWaveGamemode::HandleEnemyKilled);
}

void AWaveGamemode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);   

	if (!NewPlayer)
	{
		return;
	}
	PlayerCharacter = Cast<ACharacterBase>(NewPlayer->GetPawn());

}

void AWaveGamemode::BeginPlay()
{
	
}

void AWaveGamemode::HandleEnemyKilled(float XPReward, float SoulsReward)
{
	if (!PlayerCharacter)
	
	{
		return; 
	}

	UAttributeSet* PlayerAttributes = PlayerCharacter->GetAttributeSet();
	if (!PlayerAttributes) 
	{
		return;
	}

	const float CurrentXP = PlayerAttributes->GetAttributeValue(PlayerXPTag);
	PlayerAttributes->SetBaseValue(PlayerXPTag, CurrentXP + XPReward, ERecalcContext::LevelUp);

	const float CurrentSouls = PlayerAttributes->GetAttributeValue(PlayerSoulsTag);
	PlayerAttributes->SetBaseValue(PlayerSoulsTag, CurrentSouls + SoulsReward, ERecalcContext::LevelUp);
}
