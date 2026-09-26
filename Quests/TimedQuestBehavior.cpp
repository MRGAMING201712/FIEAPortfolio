// Fill out your copyright notice in the Description page of Project Settings.


#include "TimedQuestBehavior.h"
#include "QuestManagerSubsystem.h"

void UTimedQuestBehavior::OnQuestStarted()
{
	if (!Manager || TimeLimit <= 0.0f)
	{
		return;
	}

	UWorld* World = Manager->GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		TimeLimitHandle,
		this,
		&UTimedQuestBehavior::HandleTimeExpired,
		TimeLimit,
		false);
}

void UTimedQuestBehavior::OnQuestCompleted()
{
	ClearTimer();
}

void UTimedQuestBehavior::OnQuestFailed()
{
	ClearTimer();
}

void UTimedQuestBehavior::HandleTimeExpired()
{
	if (Manager)
	{
		Manager->FailQuest(QuestID);
	}
}

void UTimedQuestBehavior::ClearTimer()
{
	if (Manager)
	{
		if (UWorld* World = Manager->GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimeLimitHandle);
		}
	}
}
