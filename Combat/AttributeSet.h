// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeSetDefinition.h"
#include "TimerManager.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayTag, AttributeTag, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnResourceChanged, FGameplayTag, AttributeTag, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeDepleted, FGameplayTag, AttributeTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeRestoredFromZero, FGameplayTag, AttributeTag);


UENUM(BlueprintType)
enum class EModifierOperation : uint8
{
	Additive,
	Multiplicative,
	Override
};

UENUM(BlueprintType)
enum class ERecalcContext : uint8
{
	LevelUp,
	Equipment,
	Buff,
	Respec,
	Init
};

UENUM(BlueprintType)
enum class EMaxChangePolicy : uint8
{
	PreserveRatio,
	PreserveAbsolute,
	FillToMax
};

USTRUCT(BlueprintType)
struct FAttributeModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	FGameplayTag TargetAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	EModifierOperation Operation = EModifierOperation::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float Magnitude = 0.0f;

	//0 = permanent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TObjectPtr<UObject> Source = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	FGuid Handle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	FGameplayTagContainer BlockedByTags;
};

USTRUCT(BlueprintType)
struct FInstantChange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instant Change")
	FGameplayTag AttributeTag;

	//Make it negative to remove, positive to add
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instant Change")
	float Delta = 0.0f;

	// Empty = nothing blocks this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instant Change")
	FGameplayTagContainer BlockedByTags;
};

/*
 BaseValue    - the permanent stat. Level ups 
 ResolvedBase -  curve evaluated, or BaseValue when the attribute isn't derived
 CurrentValue - for resources, how much is left
 */

USTRUCT(BlueprintType)
struct FAttributeInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float BaseValue = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float ResolvedBase = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float CurrentValue = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bWasDepleted = false;
};

USTRUCT(BlueprintType)
struct FAttributeModifierArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TArray<FAttributeModifier> Modifiers;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIEAPORTFOLIO_API UAttributeSet : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeSet();

	//Delegates 
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnAttributeChanged OnAttributeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnResourceChanged OnResourceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnAttributeDepleted OnAttributeDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnAttributeRestoredFromZero OnAttributeRestoredFromZero;



	//Reads 

	//The number gameplay should use.
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetAttributeValue(FGameplayTag AttributeTag) const;

	//Final value: (ResolvedBase + Additive) * (1 + Multiplicative),Override wins, then clamped.
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetResolvedValue(FGameplayTag AttributeTag) const;

	//For health/stamina bars.
	UFUNCTION(BlueprintPure, Category = "Attributes")
	void GetResourceValues(FGameplayTag AttributeTag, float& OutCurrent, float& OutMax) const;

	//Writes
	UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
	FGuid ApplyModifier(FGameplayTag TargetAttribute, EModifierOperation Operation, float Magnitude, float Duration = 0.0f, UObject* Source = nullptr, ERecalcContext Context = ERecalcContext::Equipment, FGameplayTagContainer BlockedByTags = FGameplayTagContainer());
	UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
	bool RemoveModifier(FGameplayTag TargetAttribute, FGuid ModifierHandle, ERecalcContext Context = ERecalcContext::Equipment);

	//Damage, healing, stamina drain.
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyInstantChange(const FInstantChange& Struct);

	//Level up / stat spend.
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetBaseValue(FGameplayTag AttributeTag, float NewBase, ERecalcContext Context = ERecalcContext::LevelUp);

	//Immunity
	UFUNCTION(BlueprintCallable, Category = "Attributes|Immunity")
	void AddImmunityTag(FGameplayTag AttributeTag);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Immunity")
	void RemoveImmunityTag(FGameplayTag AttributeTag);


	//Config 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UAttributeSetDefinition> DefinitionTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes|Derivation")
	int32 MaxDerivationDepth = 8;

	//Copy of DefinitionTable->Attributes, in BeginPlay
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<FGameplayTag, FAttributeData> Definitions;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Refreshes ResolvedBase, broadcasts, through recursion does it to every attribute derived from this one. Handles all attributes, derived and non derived
	void RecalculateAttribute(FGameplayTag AttributeTag, ERecalcContext Context, int32 Depth = 0, float KnownOldResolved = -1.0f);

	void ApplyMaxChangePolicy(FGameplayTag AttributeTag, float OldMax, float NewMax, ERecalcContext Context);

	void BuildDependencyMap();

	void CheckDepletionEdge(FGameplayTag AttributeTag);

	bool CanApplyChange(FGameplayTagContainer BlockedTags) const;

	//Logs any attribute that loops back to itself
	bool ValidateNoCycles() const;

	UFUNCTION()
	void ExpireModifier(FGameplayTag TargetAttribute, FGuid ModifierHandle);

	//State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<FGameplayTag, FAttributeInstance> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Immunity")
	FGameplayTagContainer ActiveImmunityTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Modifiers")
	TMap<FGameplayTag, FAttributeModifierArray> ActiveModifiers;

	TMap<FGuid, FTimerHandle> ActiveTimers;

	//Source tag -> every attribute derived from it.
	TMap<FGameplayTag, TArray<FGameplayTag>> DependencyMap;

	//Regen
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<FGameplayTag, float> LastDecreaseTimestamp;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RegenTick();

	UPROPERTY()
	float RegenTickInterval = 0.1f;

	UPROPERTY()
	FTimerHandle RegenTimerHandle;
};