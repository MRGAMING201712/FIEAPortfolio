// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfHealAbilityEffect.h"
#include "CharacterBase.h"

void USelfHealAbilityEffect::Execute(AActor* Caster, UAbilityDefinition* Definition)
{

	ACharacterBase* Target = Cast<ACharacterBase>(Caster);

	if (!Target)
	{
		return;
	}

	if (UAttributeSet* Attributes = Target->GetAttributeSet())
	{
		FInstantChange InstantChangeStruct;
		InstantChangeStruct.AttributeTag = TargetAttribute;
		InstantChangeStruct.Delta = HealAmount;
		Attributes->ApplyInstantChange(InstantChangeStruct);
	}
}
