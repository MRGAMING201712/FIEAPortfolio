// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "QuestBehavior.generated.h"

class UQuestManagerSubsystem;
UCLASS()
class FIEAPORTFOLIO_API UQuestBehavior : public UObject
{
	GENERATED_BODY()

public:

	virtual void Initialize(UQuestManagerSubsystem* InManager, FName InQuestID);
	virtual void OnQuestStarted() {}
	virtual void OnQuestCompleted() {}
	virtual void OnQuestFailed() {}
	virtual void OnObjectiveCompleted(FName ObjectiveID) {}
	virtual bool ShouldObjectiveProgress(FName ObjectiveID, FGameplayTag EventTag) { return true;}

protected:
	UPROPERTY()
	TObjectPtr<UQuestManagerSubsystem> Manager;

	FName QuestID;


};
