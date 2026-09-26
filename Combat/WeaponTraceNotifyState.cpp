// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTraceNotifyState.h"
#include "ComboComponent.h"


void UWeaponTraceNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_01"));
		return;
	}

	UComboComponent* Combo = MeshComp->GetOwner()->FindComponentByClass<UComboComponent>();
	if (!Combo)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_02"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("WeaponTraceStart"));
	Combo->OnTraceWindowBegin();
}

void UWeaponTraceNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) 
	{
		return;
	}

	UComboComponent* Combo = MeshComp->GetOwner()->FindComponentByClass<UComboComponent>();
	if (!Combo)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_03"));
		return;
	}
		Combo->OnTraceWindowEnd();
}
