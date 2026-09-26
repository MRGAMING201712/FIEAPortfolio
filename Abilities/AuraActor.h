// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AuraActor.generated.h"


UCLASS()
class FIEAPORTFOLIO_API AAuraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Defaults
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TickInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AmountPerTick = -5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag TargetAttributeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAffectsEnemies = true; //in case an enemy shoots something that should only damage the player

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> VFX;

	FTimerHandle TickHandle;
	FTimerHandle LifetimeHandle;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AuraTick();
	void ExpireAura();

};
