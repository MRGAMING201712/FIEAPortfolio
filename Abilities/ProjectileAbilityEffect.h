// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "ProjectileAbilityEffect.generated.h"

class AProjectileActor;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FIEAPORTFOLIO_API UProjectileAbilityEffect : public UAbilityEffect
{
	GENERATED_BODY()
public:

	virtual void Execute(AActor* Caster, UAbilityDefinition* Definition) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) 
	TSubclassOf<AProjectileActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 40.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FGameplayTag AttributeTagToDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName SocketName = TEXT("hand_r");
};
