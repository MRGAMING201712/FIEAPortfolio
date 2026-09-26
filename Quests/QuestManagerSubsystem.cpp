// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSubsystem.h"
#include "QuestBehavior.h"
#include "QuestDefinition.h"

bool UQuestManagerSubsystem::StartQuest(UQuestDefinition* Definition)
{
	if (!Definition || !Definition->QuestID.IsValid())
	{
		return false;
	}

	const FName QuestID = Definition->QuestID;

	if (ActiveQuests.Contains(QuestID))
	{
		return false;
	}

	//This builds the runtime instance, prevents modifying the actual data asset
	FQuestInstance NewInstance;
	NewInstance.Definition = Definition;
	NewInstance.State = EQuestState::Active;

	//Adds objectives with their states to instance
	for (const FObjectiveDefinition& ObjDef : Definition->Objectives)
	{
		FObjectiveInstance ObjInstance;
		ObjInstance.ObjectiveID = ObjDef.ObjectiveID;
		ObjInstance.CurrentProgress = 0;

		//No Prereq means it is active
		ObjInstance.State = ObjDef.PrerequisiteObjectiveIDs.Num() == 0 ? EObjectiveState::Active : EObjectiveState::Inactive;
		
		NewInstance.Objectives.Add(ObjInstance);
	}

	ActiveQuests.Add(QuestID, NewInstance);
	
	if (Definition->BehaviorClass && Definition->BehaviorClass->IsChildOf(UQuestBehavior::StaticClass()))
	{
		UQuestBehavior* Behavior = NewObject<UQuestBehavior>(this, Definition->BehaviorClass);
		if (Behavior)
		{
			ActiveBehaviors.Add(QuestID, Behavior);
			Behavior->Initialize(this, QuestID);
		}
	}

	//Broadcast
	OnQuestStateChanged.Broadcast(QuestID, EQuestState::Active);

	for (const FObjectiveInstance& ObjInstance : ActiveQuests[QuestID].Objectives)
	{
		if (ObjInstance.State == EObjectiveState::Active)
		{
			OnObjectiveStateChanged.Broadcast(QuestID, ObjInstance.ObjectiveID, EObjectiveState::Active);
		}
	}

	//Prevents another quest behavior from messing up broadcasts
	if (UQuestBehavior* Behavior = ActiveBehaviors.FindRef(QuestID))
	{
		Behavior->OnQuestStarted();
	}

	return true;
}

void UQuestManagerSubsystem::NotifyEvent(FGameplayTag EventTag, int32 Amount, AActor* Instigator)
{
	if (!EventTag.IsValid() || Amount <= 0)
	{
		return;
	}

	struct FPendingProgress
	{
		FName QuestID;
		FName ObjectiveID;
	};

	TArray<FPendingProgress> Pending;

	for (const TPair<FName, FQuestInstance>& Pair : ActiveQuests)
	{
		const FQuestInstance& Quest = Pair.Value;

		if (Quest.State != EQuestState::Active || !Quest.Definition)
		{
			continue;
		}

		for (const FObjectiveInstance& ObjInstance : Quest.Objectives)
		{
			if (ObjInstance.State != EObjectiveState::Active)
			{
				continue;
			}

			const FObjectiveDefinition* ObjDef = FindObjectiveDefinition(Quest, ObjInstance.ObjectiveID);
			if (!ObjDef)
			{
				continue;
			}

			//Should catch child tags too : If Listening for Event.Kill should also catch Even.Kill.Skeleton
			if (!EventTag.MatchesTag(ObjDef->EventTag))
			{
				continue;
			}

			//Quest behavior can do stuff too
			if (UQuestBehavior* Behavior = ActiveBehaviors.FindRef(Pair.Key))
			{
				if (!Behavior->ShouldObjectiveProgress(ObjInstance.ObjectiveID, EventTag))
				{
					continue;
				}
			}

			Pending.Add({ Pair.Key, ObjInstance.ObjectiveID });
		}
	}

	for (const FPendingProgress& Entry : Pending)
	{
		AddObjectiveProgress(Entry.QuestID, Entry.ObjectiveID, Amount);
	}
}

bool UQuestManagerSubsystem::AddObjectiveProgress(FName QuestID, FName ObjectiveID, int32 Amount)
{
	FQuestInstance* Quest = ActiveQuests.Find(QuestID);
	if (!Quest || Quest->State != EQuestState::Active || !Quest->Definition)
	{
		return false;
	}

	FObjectiveInstance* ObjInstance = Quest->Objectives.FindByPredicate([ObjectiveID](const FObjectiveInstance& Obj)
	{
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (!ObjInstance || ObjInstance->State != EObjectiveState::Active)
	{
		return false;
	}

	const FObjectiveDefinition* ObjDef = FindObjectiveDefinition(*Quest, ObjectiveID);
	if (!ObjDef)
	{
		return false;
	}

	const int32 Required = ObjDef->RequiredCount;
	const int32 OldProgress = ObjInstance->CurrentProgress;

	ObjInstance->CurrentProgress = FMath::Clamp(OldProgress + Amount, 0, Required);

	if (ObjInstance->CurrentProgress == OldProgress)
	{
		return false;
	}

	OnObjectiveUpdated.Broadcast(QuestID, ObjectiveID, ObjInstance->CurrentProgress, Required);

	if (ObjInstance->CurrentProgress >= Required)
	{
		ObjInstance->State = EObjectiveState::Complete;
		OnObjectiveStateChanged.Broadcast(QuestID, ObjectiveID, EObjectiveState::Complete);

		HandleObjectiveCompleted(QuestID, ObjectiveID);
	}

	return true;
}

void UQuestManagerSubsystem::FailQuest(FName QuestID)
{
	FQuestInstance* Quest = ActiveQuests.Find(QuestID);
	
	if (!Quest || Quest->State != EQuestState::Active)
	{
		return;
	}

	Quest->State = EQuestState::Failed;

	for (FObjectiveInstance& ObjInstance : Quest->Objectives)
	{
		if (ObjInstance.State == EObjectiveState::Active || ObjInstance.State == EObjectiveState::Inactive)
		{
			ObjInstance.State = EObjectiveState::Failed;
		}
	}

	OnQuestStateChanged.Broadcast(QuestID, EQuestState::Failed);

	if (UQuestBehavior* Behavior = ActiveBehaviors.FindRef(QuestID))
	{
		Behavior->OnQuestFailed();
		ActiveBehaviors.Remove(QuestID);
	}
}

void UQuestManagerSubsystem::CheckQuestCompletion(FName QuestID)
{
	FQuestInstance* Quest = ActiveQuests.Find(QuestID);

	if (!Quest || Quest->State != EQuestState::Active)
	{
		return;
	}

	for (const FObjectiveInstance& ObjectiveInstance : Quest->Objectives)
	{
		if (ObjectiveInstance.State != EObjectiveState::Complete)
		{
			return; //still has work to do
		}

		Quest->State = EQuestState::Complete;
		OnQuestStateChanged.Broadcast(QuestID, EQuestState::Complete);

		if (UQuestBehavior* Behavior = ActiveBehaviors.FindRef(QuestID))
		{
			Behavior->OnQuestCompleted();
			ActiveBehaviors.Remove(QuestID);
		}
	}
}

void UQuestManagerSubsystem::HandleObjectiveCompleted(FName QuestID, FName ObjectiveID)
{
	if (UQuestBehavior* Behavior = ActiveBehaviors.FindRef(QuestID))
	{
		Behavior->OnObjectiveCompleted(ObjectiveID);
	}

	FQuestInstance* Quest = ActiveQuests.Find(QuestID);
	if (!Quest || Quest->State != EQuestState::Active || !Quest->Definition)
	{
		return;
	}
	TArray<FName> ToActivate;

	for (const FObjectiveInstance& Obj : Quest->Objectives)
	{
		if (Obj.State != EObjectiveState::Inactive)
		{
			continue;
		}

		const FObjectiveDefinition* Def = FindObjectiveDefinition(*Quest, Obj.ObjectiveID);
		if (!Def)
		{
			continue;
		}

		bool bAllPrereqsComplete = true;

		for (const FName& PrereqID : Def->PrerequisiteObjectiveIDs)
		{
			const FObjectiveInstance* Prereq = Quest->Objectives.FindByPredicate([PrereqID](const FObjectiveInstance& ObjInstance)
			{
				return ObjInstance.ObjectiveID == PrereqID;
			});

			if (!Prereq || Prereq->State != EObjectiveState::Complete)
			{
				bAllPrereqsComplete = false;
				break;
			}
		}

		if (bAllPrereqsComplete)
		{
			ToActivate.Add(Obj.ObjectiveID);
		}
	}

	for (const FName& ActivateID : ToActivate)
	{
		//Recheck quest in case previous behavior changes it
 		FQuestInstance* Q = ActiveQuests.Find(QuestID);
		if (!Q)
		{
			return; 
		}

		FObjectiveInstance* Objective = Q->Objectives.FindByPredicate([ActivateID](const FObjectiveInstance& ObjInstance)
		{
			return ObjInstance.ObjectiveID == ActivateID;
		});

		if (Objective && Objective->State == EObjectiveState::Inactive)
		{
			Objective->State = EObjectiveState::Active;
			OnObjectiveStateChanged.Broadcast(QuestID, ActivateID, EObjectiveState::Active);
		}
	}

	CheckQuestCompletion(QuestID);
}

const FQuestInstance* UQuestManagerSubsystem::GetQuest(FName QuestID) const
{
	return nullptr;
}

const FObjectiveDefinition* UQuestManagerSubsystem::FindObjectiveDefinition(const FQuestInstance& Quest, FName ObjectiveID) const
{
	if (!Quest.Definition)
	{
		return nullptr;
	}

	return Quest.Definition->Objectives.FindByPredicate([ObjectiveID](const FObjectiveDefinition& Def)
	{
		return Def.ObjectiveID == ObjectiveID;
	});
}
