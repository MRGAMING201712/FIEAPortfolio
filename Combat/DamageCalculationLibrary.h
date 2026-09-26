// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponDefinition.h"
#include "DamageCalculationLibrary.generated.h"



/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UDamageCalculationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Damage")
	static float CalculateWeaponDamage(AActor* Attacker, const FWeaponInstance& WeaponInstance, AActor* Target, bool bIsTwoHanded); //Two handed weapon logic is not implemeneted in the definition yet
};
