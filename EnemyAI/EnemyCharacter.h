// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "DifficultyScalable.h"
#include "StateTree.h"
#include "EnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, float, XPAmount, float, SoulsAmount);


/**
 * 
 */

UCLASS()
class AEnemyCharacter : public ACharacterBase, public IDifficultyScalable 
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

	//Delegates
	UPROPERTY(BlueprintCallable)
	FOnEnemyDeath OnEnemyDeath;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Info")
	FGameplayTag XPTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Info")
	FGameplayTag SoulsTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTree> EnemyStateTree;
	//If perception is added then rewrite the state tree so player isn't automatically there, probably get player in each state depending what it does

	//Functions
	virtual void HandleDeath() override;

	virtual void ApplyDifficultyModifiers_Implementation(const TMap<FGameplayTag, float>& Modifiers);

protected: 
	virtual void BeginPlay() override;

};
