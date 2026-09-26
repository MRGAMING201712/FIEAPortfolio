// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCastNotify.h"
#include "AbilityComponent.h"

void UAbilityCastNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	if (UAbilityComponent* AbilityComp = MeshComp->GetOwner()->FindComponentByClass<UAbilityComponent>())
	{
		AbilityComp->HandleCastNotify();
	}
}
