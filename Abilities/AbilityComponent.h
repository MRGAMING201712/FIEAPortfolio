// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityDefinition.h"
#include "AttributeSet.h"
#include "AbilityComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//Delegates
	void OnCastMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UAbilityDefinition>> EquippedAbilities;

	UPROPERTY()
	TMap<TObjectPtr<UAbilityDefinition>, float> CooldownEndTimes;



	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbility(int32 Slot);
	void CancelCast();
	void HandleCastNotify();

	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool IsCasting() const;

	//For UI
	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool CanActivate(int32 Slot, float& OutCooldownRemaining) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveCastMontage;
	
	int32 PendingSlot = -1;
	bool bIsCasting = false;
};
