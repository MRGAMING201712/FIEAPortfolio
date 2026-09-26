// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraActor.h"
#include "CharacterBase.h"
#include "EnemyCharacter.h"
#include "Engine/OverlapResult.h"

// Sets default values
AAuraActor::AAuraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAuraActor::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("TickAuraSpawn"));

	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		TickHandle,
		this,
		&AAuraActor::AuraTick,
		TickInterval,
		true
	);

	GetWorldTimerManager().SetTimer(LifetimeHandle,
		this,
		&AAuraActor::ExpireAura,
		Duration,
		false
	);
}

void AAuraActor::AuraTick()
{
	TArray<FOverlapResult>  Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner()); //Caster immune to aura

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	for (const FOverlapResult& Result : Overlaps)
	{
		ACharacterBase* Target = Cast<ACharacterBase>(Result.GetActor());
		if(!Target)
		{
			UE_LOG(LogTemp, Warning, TEXT("TickAuraFail_1"));

			continue;
		}

		const bool bIsEnemy = Target->IsA<AEnemyCharacter>();
		if (bIsEnemy != bAffectsEnemies)
		{
			UE_LOG(LogTemp, Warning, TEXT("TickAuraFail_2"));

			continue;
		}

		if (UAttributeSet* Attributes = Target->GetAttributeSet())
		{
			UE_LOG(LogTemp, Warning, TEXT("TickAuraSuccess_1"));

			FInstantChange InstantChangeStruct;
			InstantChangeStruct.AttributeTag = TargetAttributeTag;
			InstantChangeStruct.Delta = AmountPerTick;
			Attributes->ApplyInstantChange(InstantChangeStruct);
		}
	}
}

void AAuraActor::ExpireAura()
{
	Destroy();
}

// Called every frame
void AAuraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

