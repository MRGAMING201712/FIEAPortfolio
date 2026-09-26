// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "AuraAbilityEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API UAuraAbilityEffect : public UAbilityEffect
{
	GENERATED_BODY()

	public:
		virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) override;

	protected:
		UPROPERTY(EditDefaultsOnly, Category = "Aura")
		FGameplayTag TargetAttribute;

		UPROPERTY(EditDefaultsOnly, Category = "Aura")
		EModifierOperation Operation = EModifierOperation::Multiplicative;

		UPROPERTY(EditDefaultsOnly, Category = "Aura")
		float Magnitude = 0.3f;

		UPROPERTY(EditDefaultsOnly, Category = "Aura")
		float Duration = 20.0f;

		FGuid ActiveHandle;
};
