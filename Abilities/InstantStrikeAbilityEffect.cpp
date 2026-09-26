// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantStrikeAbilityEffect.h"
#include "CharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"   
#include "DrawDebugHelpers.h"


void UInstantStrikeAbilityEffect::Execute(AActor* Caster, UAbilityDefinition* Definition)
{
	APawn* CasterPawn = Cast<APawn>(Caster);
	if (!CasterPawn)
	{
		return;
	}

	UWorld* World = Caster->GetWorld();
	if (!World)
	{
		return;
	}

	FVector EyeLocation;
	FRotator ViewRotation;
	CasterPawn->GetActorEyesViewPoint(EyeLocation, ViewRotation);

	const FVector TraceStart = EyeLocation;
	const FVector TraceEnd = TraceStart + (ViewRotation.Vector() * MaxTargetDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Caster);
	
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params
	);
	//Draw a debug line
#if WITH_EDITOR
	DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 3.0f, 0, 1.5f);
	if (bHit)
	{
		DrawDebugSphere(World, Hit.ImpactPoint, 15.0f, 12, FColor::Green, false, 3.0f);
		UE_LOG(LogTemp, Warning, TEXT("Strike trace hit: %s"), *GetNameSafe(Hit.GetActor()));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Strike trace hit nothing"));
	}
#endif

	FVector StrikeLocation = bHit ? Hit.ImpactPoint : TraceEnd;

	FVector GroundStart = StrikeLocation + FVector(0, 0, 200.0f);
	FVector GroundEnd = StrikeLocation - FVector(0, 0, 1000.0f);
	FHitResult GroundHit;
	if (World->LineTraceSingleByChannel(GroundHit, GroundStart, GroundEnd, ECC_Visibility, Params))
	{
		StrikeLocation = GroundHit.ImpactPoint;
	}

	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			ImpactVFX,
			StrikeLocation,
			FRotator::ZeroRotator
		);
	}

	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(Caster);

	World->OverlapMultiByChannel(
		Overlaps,
		StrikeLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		OverlapParams
	);

	TArray<AActor*> DamagedActors;

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		ACharacterBase* Target = Cast<ACharacterBase>(HitActor);
		if (!Target)
		{
			continue;
		}

		UAttributeSet* TargetAttributes = Target->GetAttributeSet();
		if (!TargetAttributes)
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		FInstantChange InstantChangeStruct;
		InstantChangeStruct.AttributeTag = AttributeTagToDamage;
		InstantChangeStruct.Delta = Damage;
		InstantChangeStruct.BlockedByTags = Definition->BlockedByTags;

		TargetAttributes->ApplyInstantChange(InstantChangeStruct);
	}
	//Draw debug sphere
#if WITH_EDITOR
	DrawDebugSphere(World, StrikeLocation, Radius, 16, FColor::Yellow, false, 2.0f);
#endif

}
