// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "DifficultyManager.generated.h"

/**
 * 
 */

class UDifficultyProfile;

UCLASS()
class FIEAPORTFOLIO_API UDifficultyManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	TMap<FGameplayTag, float> GetFinalModifiers(UDifficultyProfile* Profile, int32 WaveIndex);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Debug")
	void DebugTesting(UDifficultyProfile* Profile, int32 WaveIndex);


};
