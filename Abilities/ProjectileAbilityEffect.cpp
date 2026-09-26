// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAbilityEffect.h"
#include "CharacterBase.h"
#include "ProjectileActor.h"

void UProjectileAbilityEffect::Execute(AActor* Caster, UAbilityDefinition* Definition)
{
	ACharacterBase* CasterCharacter = Cast<ACharacterBase>(Caster);
	if (!CasterCharacter || !ProjectileClass)
	{
		return;
	}

	UWorld* World = Caster->GetWorld();
	if (!World)
	{
		return;
	}

	FVector SpawnLocation;
	if (USkeletalMeshComponent* Mesh = CasterCharacter->GetMesh())
	{
		if (Mesh->DoesSocketExist(SocketName))
		{
			SpawnLocation = Mesh->GetSocketLocation(SocketName);
		}
		else
		{
			SpawnLocation = Caster->GetActorLocation() + (Caster->GetActorForwardVector() * 80.0f);
		}
	}

	FRotator SpawnRotation = Caster->GetActorRotation();
	if (APawn* CasterPawn = Cast<APawn>(Caster))
	{
		SpawnRotation = CasterPawn->GetControlRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Caster;
	SpawnParams.Instigator = Cast<APawn>(Caster);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectileActor* Projectile = World->SpawnActor<AProjectileActor>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		Projectile->InitializeProjectile(
			Damage,
			AttributeTagToDamage,
			Definition ? Definition->BlockedByTags : FGameplayTagContainer()
		);
	}

}
