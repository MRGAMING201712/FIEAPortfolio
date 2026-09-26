 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoftTargetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API USoftTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoftTargetComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Functions
	UFUNCTION()
	AActor* FindAttackTarget(FVector PreferredDirection) const;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting Component")
	float MaxTargetDistance = 600.0f; //How far a lunge can go

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting Component")
	float MaxTargetAngle = 90.0f; //Degrees off the preferred direction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting Component")
	float AngleWeight = 2.0f; //How much more direction is valued over distance, if it is above 1 it favors who you are looking at and not who is closer


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
