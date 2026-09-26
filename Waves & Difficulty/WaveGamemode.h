// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaveSpawnerComponent.h"
#include "GameplayTagContainer.h"
#include "CharacterBase.h"
#include "WaveGamemode.generated.h"

/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API AWaveGamemode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void RegisterWaveSpawner(UWaveSpawnerComponent* Spawner);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FGameplayTag PlayerXPTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FGameplayTag PlayerSoulsTag;

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleEnemyKilled(float XPReward, float SoulsReward);

	UPROPERTY()
	ACharacterBase* PlayerCharacter = nullptr;

	UPROPERTY()
	UWaveSpawnerComponent* WaveSpawner;
};
