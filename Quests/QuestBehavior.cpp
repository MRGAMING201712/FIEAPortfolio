// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestBehavior.h"

void UQuestBehavior::Initialize(UQuestManagerSubsystem* InManager, FName InQuestID)
{
	Manager = InManager;
	QuestID = InQuestID;
}
