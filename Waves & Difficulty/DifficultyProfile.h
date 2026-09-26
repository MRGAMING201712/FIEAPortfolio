// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DifficultyProfile.generated.h"

/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UDifficultyProfile : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	UCurveFloat* ProgressionCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	TMap<FGameplayTag, float> BaseModifiers;
};
