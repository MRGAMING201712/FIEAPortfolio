// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "LevelingComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelGained, int32, LevelsGained);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeLeveled,FGameplayTag, AttributeTag, int32, NewAttributeLevel);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API ULevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelingComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnLevelGained OnLevelGained;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeLeveled OnAttributeLeveled;

	//Variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	TObjectPtr<UCurveFloat> XPToNextLevelCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	TArray<FGameplayTag> LevelableAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	FGameplayTag XPTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	FGameplayTag CurrentLevelTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	FGameplayTag LevelUpPointsTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling")
	int32 PointsPerLevelUp = 1;

	UPROPERTY()
	UAttributeSet* AttributeSet;

	//Functions
	UFUNCTION(BlueprintCallable, Category = "Leveling")
	bool TryLevelUpAttribute(FGameplayTag AttributeTag);

	UFUNCTION(BlueprintPure, Category = "Leveling")
	bool CanLevelUpAttribute(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintPure, Category = "Leveling")
	void GetXPProgress(float& OutCurrent, float& OutRequired) const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Functions
	UFUNCTION()
	void OnAttributeChanged(FGameplayTag AttributeTag, float OldValue, float NewValue);

	//Variables
	bool bProcessingLevelUp = false;

};
