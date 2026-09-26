// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "WeaponDefinition.generated.h"

class AWeaponActor;


USTRUCT(BlueprintType)
struct FWeaponScalingGrade
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Coefficient = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeaponStatRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier; //Attacks later in chain can do more damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoiseDamage; //In case poise is implemented

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedByTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsesSoftTargeting;


};


/**
 * 
 */
UCLASS()
class FIEAPORTFOLIO_API UWeaponDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TMap<FGameplayTag, float> BaseDamage; //TMap supports multiple damage types

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TArray<FWeaponScalingGrade> ScalingGrades;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TArray<FWeaponStatRequirement> StatRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	int32 MaxUpgradeLevel = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TObjectPtr<UCurveFloat> UpgradeCostCurve;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TObjectPtr<UCurveFloat> DamageUpgradeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TObjectPtr<UCurveFloat> ScalingUpgradeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	FName EquippedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	FName SheatherSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	FTransform AttachOffset;

	UPROPERTY(EditAnywhere, Category = "Weapon Definition")
	TSubclassOf<AWeaponActor> WeaponActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TArray<FAttackData> LightChainData; //Light Attacks

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	TArray<FAttackData> HeavyChainData; //Heavy Attacks

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Definition")
	FGameplayTag TwoHandedBonusAttribute;
};
