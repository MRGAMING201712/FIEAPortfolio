// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAbilityEffect.h"
#include "CharacterBase.h"
#include "GameFramework/Character.h"


void UAuraAbilityEffect::Execute(AActor* Caster, UAbilityDefinition* Definition)
{
	ACharacterBase* Target = Cast<ACharacterBase>(Caster);
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast"));

		return;
	}

	UAttributeSet* Attributes = Target->GetAttributeSet();
	if (!Attributes)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute"));

		return;
	}

	if (ActiveHandle.IsValid())
	{
		Attributes->RemoveModifier(TargetAttribute, ActiveHandle, ERecalcContext::Buff);
	}

	UE_LOG(LogTemp, Warning, TEXT("SUCCESS"));


	ActiveHandle = Attributes->ApplyModifier(
		TargetAttribute,
		Operation,
		Magnitude,
		Duration,
		this,
		ERecalcContext::Buff
	);
}
