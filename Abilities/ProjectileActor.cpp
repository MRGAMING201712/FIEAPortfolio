// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "CharacterBase.h"
#include "AttributeSet.h"


// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(Radius);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SetRootComponent(CollisionSphere);

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFX->SetupAttachment(CollisionSphere);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = 5.0f;
}

void AProjectileActor::InitializeProjectile(float InDamage, FGameplayTag InAttributeTagToDamage, const FGameplayTagContainer& InBlockedByTags)
{
	Damage = InDamage;
	AttributeTagToDamage = InAttributeTagToDamage;
	BlockedByTags = InBlockedByTags;
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileActor::OnSphereOverlap);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AProjectileActor::OnProjectileStop);

	if (AActor* MyOwner = GetOwner())
	{
		CollisionSphere->IgnoreActorWhenMoving(MyOwner, true);
	}
}

void AProjectileActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasHit || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ProjectileActor"));

	bHasHit = true;

	const FVector ImpactPoint = bFromSweep ? FVector(SweepResult.ImpactPoint) : GetActorLocation();
	const FRotator ImpactRotation = bFromSweep ? SweepResult.ImpactNormal.Rotation() : GetActorRotation();

	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactVFX, ImpactPoint, ImpactRotation);
	}

	if (ACharacterBase* Target = Cast<ACharacterBase>(OtherActor))
	{
		if (UAttributeSet* TargetAttributes = Target->GetAttributeSet())
		{
			FInstantChange InstantChangeStruct;
			InstantChangeStruct.AttributeTag = AttributeTagToDamage;
			InstantChangeStruct.Delta = Damage;
			InstantChangeStruct.BlockedByTags = BlockedByTags;
		}
	}

	Destroy();

}

void AProjectileActor::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (bHasHit)
	{
		return;
	}

	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), 
			ImpactVFX, 
			ImpactResult.ImpactPoint,
			FVector(ImpactResult.ImpactNormal).Rotation()
		);
	}

	Destroy();
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


