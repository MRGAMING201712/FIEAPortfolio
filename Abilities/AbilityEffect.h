// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AttributeSet.h"
#include "AbilityEffect.generated.h"

class UAbilityDefinition;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API UAbilityEffect : public UObject
{
	GENERATED_BODY()
public:
	virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) {};
	

};
