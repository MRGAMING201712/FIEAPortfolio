// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionStateComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	None,
	Attacking,
	Casting,
	Dodging,
	Staggered,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, ECharacterActionState, OldState, ECharacterActionState, NewState);




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIEAPORTFOLIO_API UActionStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionStateComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Delegate
	UPROPERTY(BlueprintAssignable, Category = "Character State")
	FOnActionStateChanged OnActionStateChanged;

	//State Changes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	ECharacterActionState CurrentState;

	UFUNCTION(BlueprintPure, Category = "Character State")
	bool CanEnterState(ECharacterActionState NewState) const;

	bool TryEnterState(ECharacterActionState NewState);

	void ForceEnterState(ECharacterActionState NewState); //primarily for death but can be used anywhere but is meant for a hole in the normal rules

	UFUNCTION(BlueprintPure, Category = "Character State")
	ECharacterActionState GetCurrentState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
