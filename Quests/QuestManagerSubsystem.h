// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestDefinition.h"
#include "GameplayTagContainer.h"
#include "QuestManagerSubsystem.generated.h"

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnObjectiveUpdated, FName, QuestID, FName, ObjectiveID, int32, Progress, int32, Required);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnObjectiveStateChanged, FName, QuestID, FName, ObjectiveID, EObjectiveState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, FName, QuestID, EQuestState, NewState);

UCLASS()
class FIEAPORTFOLIO_API UQuestManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnObjectiveUpdated OnObjectiveUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnObjectiveStateChanged OnObjectiveStateChanged; 

	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestStateChanged OnQuestStateChanged;

	UPROPERTY()
	TMap<FName, FQuestInstance> ActiveQuests;

	UPROPERTY()
	TMap<FName, TObjectPtr<UQuestBehavior>> ActiveBehaviors;

	//Main Event
	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool StartQuest(UQuestDefinition* Definition);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void NotifyEvent(FGameplayTag EventTag, int32 Amount = 1, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool AddObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void FailQuest(FName QuestID);

	void CheckQuestCompletion(FName QuestID);
	void HandleObjectiveCompleted(FName QuestID, FName ObjectiveID);
	

	//Getter
	const FQuestInstance* GetQuest(FName QuestID) const;
	const FObjectiveDefinition* FindObjectiveDefinition(const FQuestInstance& Quest, FName ObjectiveID) const;

	//UFUNCTION(BlueprintPure, Category = "Quests")
	//bool GetQuestState(FName QuestID, EQuestState& OutState) const;
protected:

};
