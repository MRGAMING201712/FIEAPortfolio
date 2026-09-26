// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttributeSet.h"
#include "ActionStateComponent.h"
#include "EquipmentComponent.h"
#include "ComboComponent.h"
#include "AbilityComponent.h"
#include "SoftTargetComponent.h"
#include "MotionWarpingComponent.h"
#include "CharacterBase.generated.h"

UCLASS()
class FIEAPORTFOLIO_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UActionStateComponent> ActionState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UComboComponent> Combo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAbilityComponent> Abilities;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USoftTargetComponent> SoftTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	bool bTwoHanded = false;// If two handing gets implemented

	//Shared Functions
	UFUNCTION(BlueprintCallable, Category = "Character Functions")
	virtual bool TryAttack(bool bHeavy);

	UFUNCTION(BlueprintCallable, Category = "Character Functions")
	virtual bool TryCastAbility(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Character Functions")
	virtual bool TryDodge(FVector Direction); //WIP

	virtual FVector GetPreferredTargetDirection() const; //This is for motion warping

	//Attribute Set Getter
	UFUNCTION(BlueprintPure, Category = "Components")
	UAttributeSet* GetAttributeSet();

	UFUNCTION(BlueprintPure, Category = "Components")
	UActionStateComponent* GetActionState();

	UFUNCTION(BlueprintPure, Category = "Components")
	UMotionWarpingComponent* GetMotionWarping();

	UFUNCTION(BlueprintPure, Category = "Components")
	USoftTargetComponent* GetSoftTarget();

	UFUNCTION(BlueprintPure, Category = "Components")
	UComboComponent* GetCombo();

	//Team Filter
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	int32 TeamID = 0; //0 is Player, 1 is Enemy, other numbers if more teams are added

	UFUNCTION(BlueprintPure, Category = "Team")
	int32 GetTeamID() const;

	//Orbiting Around Target
	UFUNCTION(BlueprintPure, Category = "AI")
	float GetOrbitAngleOffset() const;

	void SetOrbitAngleOffset(float NewOffset);

	//Delegates
	UFUNCTION()
	void HandleAttributeDepleted(FGameplayTag AttributeTag);

	UFUNCTION()
	void HandleActionStateChanged(ECharacterActionState OldState, ECharacterActionState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FGameplayTag HealthAttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponDefinition> StartingWeapon; //Added to the enemy instantly, can be added to player if you choose to add a starting weapon

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void HandleDeath();
	virtual void HandleStagger(float PoiseDamage); //WIP

	//Orbiting Around Target
	UPROPERTY()
	float OrbitAngleOffset = 0.0f;
	bool bOrbitAngleAssigned = false;
};
