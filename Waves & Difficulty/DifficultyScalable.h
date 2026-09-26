// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "DifficultyScalable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDifficultyScalable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIEAPORTFOLIO_API IDifficultyScalable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Difficulty")
	void ApplyDifficultyModifiers(const TMap<FGameplayTag, float>& Modifiers);
};
