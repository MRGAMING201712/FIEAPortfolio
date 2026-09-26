// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboWindowNotifyState.h"
#include "ComboComponent.h"

void UComboWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) 
	{
		return;
	}

	if (UComboComponent* Combo = MeshComp->GetOwner()->FindComponentByClass<UComboComponent>())
	{
		Combo->OnComboWindowOpen();
	}
}

void UComboWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) 
	{
		return;
	}

	if (UComboComponent* Combo = MeshComp->GetOwner()->FindComponentByClass<UComboComponent>())
	{
		Combo->OnComboWindowClosed();
	}
}
