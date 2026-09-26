// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.h"
#include "AttributeSet.h"
#include "ComboComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComboComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float InputBufferWindow = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	FGameplayTag StaminaAttributeTag;



	//Functions
	UFUNCTION(BlueprintCallable)
	bool RequestAttack(bool bHeavy);
	void ResetCombo();

	void OnComboWindowOpen();
	void OnComboWindowClosed();

	void OnTraceWindowBegin();
	void OnTraceWindowEnd();

	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsAttacking() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool TryAdvanceCombo(); //increment then start the attack
	bool StartAttackAtIndex(int32 Index, bool bHeavy);


	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Warping")
	float AttackReach = 60.0f; //how far past the capsules should your reach extend

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Warping")
	float MaxWarpDistance = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combo|Warping")
	FName WarpTargetName = TEXT("AttackTarget");

	int32 CurrentChainIndex;
	bool bIsAttacking;
	bool bComboWindowOpen;
	bool bCurrentAttackIsHeavy;
	float BufferedInputTime;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	//Should prevent end montage events from other attacks when blending in/out, seems to mess with the combos
	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveMontage;

};
