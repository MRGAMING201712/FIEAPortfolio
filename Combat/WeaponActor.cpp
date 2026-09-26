// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "WeaponDefinition.h"
#include "DamageCalculationLibrary.h"
#include "CharacterBase.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MeshComponent);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTraceActive)
	{
		PerformTrace();
	}
}


void AWeaponActor::InitializeFromInstance(const FWeaponInstance& InInstance)
{
	WeaponInstance = InInstance;

	if (WeaponInstance.Definition && WeaponInstance.Definition->WeaponMesh)
	{
		MeshComponent->SetSkeletalMesh(WeaponInstance.Definition->WeaponMesh);
	}
}

void AWeaponActor::SetTraceActive(bool bActive, float DamageMultiplier, const FGameplayTagContainer& BlockedByTags)
{
	bTraceActive = bActive;
	SetActorTickEnabled(bActive);

	if (bActive)
	{
		CurrentAttackDamageMultiplier = DamageMultiplier;
		CurrentAttackBlockedByTags = BlockedByTags;

		HitActorsThisSwing.Empty();

		PreviousPositions.Empty();
		const FVector Start = MeshComponent->GetSocketLocation(TraceStartSocket);
		const FVector End = MeshComponent->GetSocketLocation(TraceEndSocket);

		for (int32 i = 0; i <= TraceSegments; ++i)
		{
			const float Alpha = static_cast<float>(i) / TraceSegments;
			PreviousPositions.Add(FMath::Lerp(Start, End, Alpha));
		}
	}
}

void AWeaponActor::PerformTrace()
{
	bool bTwoHanded = false;
	ACharacterBase* MyCharacter = Cast<ACharacterBase>(GetOwner());
	if (!MyCharacter)
	{
		return;
	}

	const uint8 MyTeam = MyCharacter->GetTeamID();
	//bTwoHanded = OwnerCharacter->IsTwoHanding(); Not implemented yet

	const FVector Start = MeshComponent->GetSocketLocation(TraceStartSocket);
	const FVector End = MeshComponent->GetSocketLocation(TraceEndSocket);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	for (int32 i = 0; i <= TraceSegments; ++i)
	{
		const float Alpha = static_cast<float>(i) / TraceSegments;
		const FVector Current = FMath::Lerp(Start, End, Alpha);

		TArray<FHitResult> Hits;

		GetWorld()->SweepMultiByChannel(
			Hits,
			PreviousPositions[i],
			Current,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(TraceRadius),
			Params
		);

		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActorsThisSwing.Contains(HitActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_02"));

				continue;
			}

			HitActorsThisSwing.Add(HitActor);

			ACharacterBase* TargetCharacter = Cast<ACharacterBase>(HitActor);
			if (!TargetCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_03"));

				continue;
			}

			if (TargetCharacter->GetTeamID() == MyTeam)
			{
				continue;   // same team
			}

			if (UActionStateComponent* State = TargetCharacter->GetActionState())
			{
				if (State->GetCurrentState() == ECharacterActionState::Dead)
				{
					continue;
				}
			}

			UAttributeSet* TargetAttributes = TargetCharacter->GetAttributeSet();
			if (!TargetAttributes)
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponTraceFail_04"));

				continue;
			}

			const float BaseDamage = UDamageCalculationLibrary::CalculateWeaponDamage(GetOwner(), WeaponInstance, HitActor, bTwoHanded);

			FInstantChange InstantChangeStruct;
			InstantChangeStruct.AttributeTag = AttributeToDamage;
			InstantChangeStruct.Delta = -(BaseDamage * CurrentAttackDamageMultiplier);
			InstantChangeStruct.BlockedByTags = CurrentAttackBlockedByTags;
			UE_LOG(LogTemp, Warning, TEXT("WeaponTraceApplyDamage"));

			TargetAttributes->ApplyInstantChange(InstantChangeStruct);
		}

		PreviousPositions[i] = Current;
	}
}
