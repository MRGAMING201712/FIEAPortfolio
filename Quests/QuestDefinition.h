// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestBehavior.h"
#include "QuestDefinition.generated.h"

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	Collect, 
	Kill, 
	Interact,
	Custom
};

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	Inactive,
	Active,
	Complete,
	Failed
};

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	Inactive,
	Active,
	Complete,
	Failed
};

USTRUCT(BlueprintType)
struct FObjectiveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName ObjectiveID;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	EObjectiveType Type = EObjectiveType::Kill;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EventTag;

	UPROPERTY(EditDefaultsOnly)
	int32 RequiredCount = 1;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> PrerequisiteObjectiveIDs;
};

USTRUCT(BlueprintType)
struct FObjectiveInstance
{
	GENERATED_BODY()

	FName ObjectiveID;
	EObjectiveState State;
	int32 CurrentProgress;
};

USTRUCT(BlueprintType)
struct FQuestInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UQuestDefinition> Definition;

	EQuestState State;
	TArray<FObjectiveInstance> Objectives;
};


UCLASS()
class FIEAPORTFOLIO_API UQuestDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FName QuestID;

	UPROPERTY(EditDefaultsOnly)
	FText QuestName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FObjectiveDefinition> Objectives;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UQuestBehavior> BehaviorClass;

protected:


};
