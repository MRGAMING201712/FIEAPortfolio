// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityEffect.h"
#include "AbilityDefinition.generated.h"



/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UAbilityDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//Universal Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	FGameplayTag CostAttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	float Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAnimMontage> CastMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UAbilityEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	FGameplayTagContainer BlockedByTags;

};
