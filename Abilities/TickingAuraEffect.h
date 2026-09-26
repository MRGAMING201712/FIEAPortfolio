// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "TickingAuraEffect.generated.h"

class AAuraActor;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API UTickingAuraEffect : public UAbilityEffect
{
	GENERATED_BODY()
	public:
		virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) override;

		UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Aura")
		TSubclassOf<AAuraActor> AuraActorClass;
	protected:

};
