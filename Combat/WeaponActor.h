// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "WeaponActor.generated.h"


UCLASS()
class FIEAPORTFOLIO_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshComponent; //Put sockets on here for start/end traces


	void InitializeFromInstance(const FWeaponInstance& InInstance);

	void SetTraceActive(bool bActive, float DamageMultiplier = 1.0f, const FGameplayTagContainer& BlockedByTags = FGameplayTagContainer());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PerformTrace();

	UPROPERTY()
	FWeaponInstance WeaponInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	FName TraceStartSocket = TEXT("TraceStart");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	FName TraceEndSocket = TEXT("TraceEnd");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	float TraceRadius = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	int32 TraceSegments = 4;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActorsThisSwing;

	TArray<FVector> PreviousPositions;
	bool bTraceActive = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	FGameplayTag AttributeToDamage;

	float CurrentAttackDamageMultiplier = 1.0f;
	FGameplayTagContainer CurrentAttackBlockedByTags;
};
