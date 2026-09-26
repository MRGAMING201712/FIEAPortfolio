// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "AttributeSetDefinition.generated.h"


USTRUCT(BlueprintType)
struct FAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float BaseValue = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxValue = 9999.0f;

	//Meter style attribute: CurrentValue is how much is left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsResource = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Derivation")
	bool bIsDerived = false;

	// The attribute this one is computed from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Derivation", meta = (EditCondition = "bIsDerived"))
	FGameplayTag SourceTag;

	//Maps SourceTag's resolved value to this attribute's ResolvedBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Derivation", meta = (EditCondition = "bIsDerived"))
	TObjectPtr<UCurveFloat> DerivationCurve = nullptr;

	// Regen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bRegenerates = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float RegenRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float RegenDelay = 1.0f;
};


UCLASS()
class FIEAPORTFOLIO_API UAttributeSetDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	//Keyed by attribute tag. The key is the attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	TMap<FGameplayTag, FAttributeData> Attributes;
};