// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "NiagaraComponent.h"
#include "InstantStrikeAbilityEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API UInstantStrikeAbilityEffect : public UAbilityEffect
{
	GENERATED_BODY()
	
public:
	virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxTargetDistance = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AttributeTagToDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> ImpactVFX;
};
