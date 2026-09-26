// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestBehavior.h"
#include "TimedQuestBehavior.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FIEAPORTFOLIO_API UTimedQuestBehavior : public UQuestBehavior
{
	GENERATED_BODY()
	
public:
	virtual void OnQuestStarted() override;
	virtual void OnQuestCompleted() override;
	virtual void OnQuestFailed() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Timed Quest")
	float TimeLimit = 10.0f;

	UFUNCTION()
	void HandleTimeExpired();

	void ClearTimer();

	FTimerHandle TimeLimitHandle;
};
