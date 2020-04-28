// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#include "Game/QuestManager.h"
#include "Kismet/GameplayStatics.h"
#include "TCStatics.h"
#include "Player/PlayerControllerBase.h"

// Sets default values
AQuestManager::AQuestManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AQuestManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AQuestManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AQuestManager::BeginQuest(int32 QuestID, bool MakeActive)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// If valid ID and not previously undertaken, spawn this quest
	if (QuestID > 0 && !CompletedQuests.Contains(QuestID) && 
		!FailedQuests.Contains(QuestID) && !ActiveQuests.Contains(QuestID))
	{
		TSubclassOf<AMasterQuest>* Elem = AllQuests.Find(QuestID);

		if (Elem)
		{
			auto Spawn = GetWorld()->SpawnActor<AMasterQuest>(*Elem);
			ActiveQuests.Add(QuestID, Spawn);

			AMasterQuest* Quest = Cast<AMasterQuest>(Spawn);

			if (Quest)
				Quest->OnBeginDelegate.Broadcast();

			// Make it our current quest if requested to
			if (MakeActive)
			{
				if (PC)
					PC->SetCurrentQuest(QuestID);
			}

			return true;
		}
	}
	else if (MakeActive && ActiveQuests.Contains(QuestID))
	{
		PC->SetCurrentQuest(QuestID);
	}
	
	return false;
}

bool AQuestManager::AdvanceQuest(int32 QuestID, bool CurrObjCompleted)
{
	// Advance quest if completed. Otherwise, fail it
	if (CurrObjCompleted)
	{
		// Check if we have the supplied quest
		auto Quest = ActiveQuests.Find(QuestID);

		if (Quest)
		{
			// Get how many objectives this quest has and what objective we're curr on
			auto Amt = (*Quest)->Objectives.Num();
			auto Curr = (*Quest)->QuestInfo.CurrentObjective;

			// Finish optional objective in case we haven't already
			FinishOptionalObjective(QuestID, false);

			// If at last objective, complete quest. Else, move to next obj and notify
			if ((Amt - 1) == Curr)
				return CompleteQuest(QuestID);
			else
			{
				(*Quest)->Objectives[Curr].Completed = true;
				(*Quest)->Objectives[Curr].IsFinished = true;
				(*Quest)->QuestInfo.IncrementCurrentObjective();
				(*Quest)->OnObjectiveAdvanceDelegate.Broadcast(--Curr, Curr);

				Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->UpdateQuestHUD(QuestID);

				return true;
			}
		}
		else
			return false;
	}
	else
		return FailQuest(QuestID);
}

bool AQuestManager::FinishOptionalObjective(int32 QuestID, bool ObjCompleted)
{
	auto Quest = ActiveQuests.Find(QuestID);

	if (Quest)
	{
		// Get current optional objective index
		int32 OptionalObjIndex = (*Quest)->Objectives[
			(*Quest)->QuestInfo.CurrentObjective].OptionalObjIndex;

		// If current optional obj isn't null, check if we completed it
		if (OptionalObjIndex != UTCStatics::DEFAULT_OBJECTIVE_ID)
		{
			auto& OptionalObj = (*Quest)->OptionalObjectives[OptionalObjIndex];

			// Check if we previously completed this objective
			if (!OptionalObj.IsFinished)
			{
				if (ObjCompleted)
				{
					// Complete the objective and initiate new quest if there's one to give
					if (OptionalObj.NewQuestOnComplete != UTCStatics::DEFAULT_OBJECTIVE_ID)
						BeginQuest(QuestID, false);

					OptionalObj.Completed = true;
				}
				else
				{
					OptionalObj.Completed = false;
				}

				OptionalObj.IsFinished = true;
			}

			return true;
		}
	}
	return false;
}

bool AQuestManager::CompleteQuest(int32 QuestID)
{
	auto Quest = ActiveQuests.Find(QuestID);

	if (Quest)
	{
		(*Quest)->OnCompletedDelegate.Broadcast();

		int32 NewQuest = (*Quest)->QuestInfo.FollowUpQuest;

		// Remove old quest, move it to completed. Begin new one (if it exists)
		if (ActiveQuests.Remove(QuestID) > 0)
		{
			CompletedQuests.Add(QuestID);
			if (NewQuest != UTCStatics::DEFAULT_QUEST_ID)
			{
				if (BeginQuest(NewQuest, true))
					return true;
			}
				
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(
					UGameplayStatics::GetPlayerController(GetWorld(), 0));

			PC->SetCurrentQuest(UTCStatics::DEFAULT_QUEST_ID, true);

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool AQuestManager::FailQuest(int32 QuestID)
{
	auto Quest = ActiveQuests.Find(QuestID);

	if (Quest)
	{
		(*Quest)->OnFailDelegate.Broadcast();

		int32 NewQuest = (*Quest)->QuestInfo.FailFollowUpQuest;

		// Remove old quest, move it to failed. Begin new one (if it exists)
		if (ActiveQuests.Remove(QuestID) > 0)
		{
			FailedQuests.Add(QuestID);
			if (NewQuest != UTCStatics::DEFAULT_QUEST_ID)
			{
				if (BeginQuest(NewQuest, true))
					return true;
			}

			APlayerControllerBase* PC = Cast<APlayerControllerBase>(
				UGameplayStatics::GetPlayerController(GetWorld(), 0));

			PC->SetCurrentQuest(UTCStatics::DEFAULT_QUEST_ID, true);

			return true;
		}
		else
			return false;
	}
	else
		return false;
}