// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageCalculationLibrary.h"
#include "WeaponActor.h"
#include "CharacterBase.h"

float UDamageCalculationLibrary::CalculateWeaponDamage(AActor* Attacker, const FWeaponInstance& WeaponInstance, AActor* Target, bool bIsTwoHanded)
{
	if (!WeaponInstance.Definition || !Attacker)
	{
		return 0.0f;
	}

	ACharacterBase* AttackerCharacter = Cast<ACharacterBase>(Attacker);
	if (!AttackerCharacter)
	{
		return 0.0f;
	}

	UAttributeSet* AttributeSet = AttackerCharacter->GetAttributeSet();
	if (!AttributeSet)
	{
		return 0.0f;
	}

	const UWeaponDefinition* WeaponDefinition = WeaponInstance.Definition;
	
	float DamageUpgradeMult = 1.0f;
	float ScalingUpgradeMult = 1.0f;

	if (WeaponDefinition->DamageUpgradeCurve)
	{
		DamageUpgradeMult = WeaponDefinition->DamageUpgradeCurve->GetFloatValue(WeaponInstance.UpgradeLevel);
	}

	if (WeaponDefinition->ScalingUpgradeCurve)
	{
		ScalingUpgradeMult = WeaponDefinition->ScalingUpgradeCurve->GetFloatValue(WeaponInstance.UpgradeLevel);
	}

	float ScalingBonus = 0.0f;

	for (const FWeaponScalingGrade& Grade : WeaponDefinition->ScalingGrades)
	{
		float StatValue = AttributeSet->GetAttributeValue(Grade.AttributeTag);

		if (bIsTwoHanded && Grade.AttributeTag == WeaponDefinition->TwoHandedBonusAttribute)
		{
			StatValue *= 1.5;
		}

		bool bMeetsRequirement = true; //if not met then that attribute is not contributing to final bonus

		for (const FWeaponStatRequirement& Requirement : WeaponDefinition->StatRequirements)
		{
			if (Requirement.AttributeTag == Grade.AttributeTag && StatValue < Requirement.MinimumValue)
			{
				bMeetsRequirement = false;
				break;
			}
		}

		if (bMeetsRequirement)
		{
			ScalingBonus += StatValue * Grade.Coefficient * ScalingUpgradeMult;
		}
	}

	float TotalBase = 0.0f;
	//Damage types and resistance are a WIP, map is there for damage types, should  be calculated sepaseparately and return a tmap for each type and then apply instant change for each type
	for (const TPair<FGameplayTag, float>& Pair : WeaponDefinition->BaseDamage)
	{
		TotalBase += Pair.Value;
	}

	return (TotalBase * DamageUpgradeMult) + ScalingBonus;
}
