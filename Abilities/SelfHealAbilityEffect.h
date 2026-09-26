// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "SelfHealAbilityEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API USelfHealAbilityEffect : public UAbilityEffect
{
	GENERATED_BODY()
	
public:
	virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag TargetAttribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HealAmount = 30.0f;


protected:


};
