// Fill out your copyright notice in the Description page of Project Settings.


#include "TickingAuraEffect.h"
#include "AuraActor.h"

void UTickingAuraEffect::Execute(AActor* Caster, UAbilityDefinition* Definition)
{

	if (!Caster || !AuraActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("TickAuraEffectFail_1"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Caster;
	SpawnParams.Instigator = Cast<APawn>(Caster);

	AAuraActor* Aura = Caster->GetWorld()->SpawnActor<AAuraActor>(
		AuraActorClass,
		Caster->GetActorTransform(),
		SpawnParams
	);

	if (Aura)
	{
		Aura->AttachToActor(Caster, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
