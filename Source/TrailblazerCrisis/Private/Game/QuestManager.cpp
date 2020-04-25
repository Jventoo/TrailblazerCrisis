// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#include "Game/QuestManager.h"
#include "Kismet/GameplayStatics.h"
#include "TCStatics.h"
#include "Player/PlayerControllerBase.h"

// Sets default values
AQuestManager::AQuestManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	TSubclassOf<AMasterQuest>* Elem = AllQuests.Find(QuestID);

	if (Elem)
	{
		auto Spawn = GetWorld()->SpawnActor<AMasterQuest>(*Elem);
		ActiveQuests.Add(QuestID, Spawn);

		AMasterQuest* Quest = Cast<AMasterQuest>(Spawn);

		if (Quest)
			Quest->OnBeginDelegate.Broadcast();

		if (MakeActive)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(
				UGameplayStatics::GetPlayerController(GetWorld(), 0));

			if (PC)
				PC->SetCurrentQuest_DEBUG(QuestID);
		}

		return true;
	}
	else
		return false;
}

bool AQuestManager::AdvanceQuest(int32 QuestID, bool CurrObjCompleted)
{
	if (CurrObjCompleted)
	{
		auto Quest = ActiveQuests.Find(QuestID);

		if (Quest)
		{
			auto Amt = (*Quest)->Objectives.Num();
			auto Curr = (*Quest)->QuestInfo.CurrentObjective;

			if (--Amt == Curr)
				return CompleteQuest(QuestID);
			else
			{
				(*Quest)->QuestInfo.IncrementCurrentObjective();
				(*Quest)->OnObjectiveAdvanceDelegate.Broadcast(--Curr, Curr);
				return true;
			}
		}
		else
			return false;
	}
	else
		return FailQuest(QuestID);
}

bool AQuestManager::FinishObjective(int32 QuestID, bool ObjCompleted)
{
	auto Quest = ActiveQuests.Find(QuestID);

	if (Quest)
	{
		const auto& Objectives = (*Quest)->Objectives;
		int32 OptionalObjIndex = Objectives[
			(*Quest)->QuestInfo.CurrentObjective].OptionalObjIndex;

		if (OptionalObjIndex != UTCStatics::DEFAULT_OBJECTIVE_ID)
		{
			const auto& OptionalObj = (*Quest)->OptionalObjectives[OptionalObjIndex];

			if (OptionalObj.Completed &&
				OptionalObj.NewQuestOnComplete != UTCStatics::DEFAULT_OBJECTIVE_ID)
				BeginQuest(QuestID, false);
		}

		return AdvanceQuest(QuestID, ObjCompleted);
	}
	else
		return false;
}

bool AQuestManager::CompleteQuest(int32 QuestID)
{
	auto Quest = ActiveQuests.Find(QuestID);

	if (Quest)
	{
		(*Quest)->OnCompletedDelegate.Broadcast();

		int32 NewQuest = (*Quest)->QuestInfo.FollowUpQuest;
		if (ActiveQuests.Remove(NewQuest) > 0)
		{
			CompletedQuests.Add(QuestID);
			if (NewQuest != UTCStatics::DEFAULT_QUEST_ID)
				return BeginQuest(NewQuest, true);
			else
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
		if (ActiveQuests.Remove(NewQuest) > 0)
		{
			FailedQuests.Add(QuestID);
			if (NewQuest != UTCStatics::DEFAULT_QUEST_ID)
				return BeginQuest(NewQuest, true);
			else
				return true;
		}
		else
			return false;
	}
	else
		return false;
}