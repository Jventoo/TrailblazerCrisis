// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Components/ObjectiveComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Game/QuestManager.h"
#include "Player/PlayerControllerBase.h"
#include "TCStatics.h"

// Sets default values for this component's properties
UObjectiveComponent::UObjectiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	tempobj = -1;
	tempopt = -1;
}


// Called when the game starts
void UObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UObjectiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	tempobj = ObjProgress.Num();
	tempopt = OptObjProgress.Num();
}

void UObjectiveComponent::ChangeCurrentQuest(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		// If we successfully update quest and aren't currently tracking it...
		if (PC && PC->SetCurrentQuest(QuestID))
		{
			UpdateObjectiveProgress(QuestID);
		}
	}
}

int32 UObjectiveComponent::GetCurrentQuest() const
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
		return PC->GetCurrentQuest();
	else
		return UTCStatics::DEFAULT_QUEST_ID;
}

bool UObjectiveComponent::BeginQuest(int32 QuestID, bool MakeActive)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		bool success = (PC->GetQuestManager()->BeginQuest(QuestID, MakeActive));

		UpdateObjectiveProgress(QuestID);
		UpdateOptionalObjectiveProgress(QuestID);

		return success;
	}
	else
		return false;
}

bool UObjectiveComponent::ProgressQuest(int32 QuestID, bool CurrCompleted)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC && ObjProgress.Contains(QuestID))
	{
		bool success = PC->GetQuestManager()->AdvanceQuest(QuestID, CurrCompleted);

		return (UpdateObjectiveProgress(QuestID) 
			&& UpdateOptionalObjectiveProgress(QuestID) && success);
	}
	else
		return false;
}

bool UObjectiveComponent::ProgressObjective(int32 QuestID, int32 ProgressIncrease)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// If we are currently tracking this quest's progress...
	if (PC && ObjProgress.Contains(QuestID))
	{
		// Get the objective struct and inc our current progress
		auto& Struct = ObjProgress[QuestID];
		Struct.IncrementProgress(ProgressIncrease);

		// If we are at or over our goal, progress the quest. Else, move on
		if (Struct.CurrentProgress >= Struct.ProgressGoal)
			return ProgressQuest(QuestID, true);
		else
			return true;
	}
	else
		return false;
}

bool UObjectiveComponent::ProgressOptionalObjective(int32 QuestID, int32 ProgressIncrease)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// If we are currently tracking this quest's progress...
	if (PC && OptObjProgress.Contains(QuestID))
	{
		// Get the objective struct and inc our current progress
		auto& Struct = OptObjProgress[QuestID];
		Struct.IncrementProgress(ProgressIncrease);

		// If we are at or over our goal, make the opt obj as completed. Else, move on
		if (Struct.CurrentProgress >= Struct.ProgressGoal)
			return PC->GetQuestManager()->FinishOptionalObjective(QuestID, true);
		else
			return true;
	}
	else
		return false;
}

bool UObjectiveComponent::FinishQuest(int32 QuestID, bool Completed)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		if (ObjProgress.Contains(QuestID))
			ObjProgress.Remove(QuestID);

		if (OptObjProgress.Contains(QuestID))
			OptObjProgress.Remove(QuestID);

		if (Completed)
			return PC->GetQuestManager()->CompleteQuest(QuestID);
		else
			return PC->GetQuestManager()->FailQuest(QuestID);
	}
	else
		return false;
}

bool UObjectiveComponent::UpdateObjectiveProgress(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC && PC->GetQuestManager()->ActiveQuests.Contains(QuestID)) // Start tracking or update tracking of active quest
	{
		const auto& Quest = PC->GetQuestManager()->ActiveQuests[QuestID];

		// Get objective stats
		FObjectiveProgress ObjProg;
		ObjProg.ObjectiveID = Quest->QuestInfo.CurrentObjective;
		ObjProg.ProgressGoal = Quest->Objectives[ObjProg.ObjectiveID].ProgressGoal;

		// Track it in our map
		ObjProgress.Emplace(QuestID, ObjProg);

		return true;
	}
	else if (ObjProgress.Contains(QuestID)) // Stop tracking progress for completed/failed quests
	{
		ObjProgress.Remove(QuestID);

		return true;
	}
	else
		return false;
}

bool UObjectiveComponent::UpdateOptionalObjectiveProgress(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC && PC->GetQuestManager()->ActiveQuests.Contains(QuestID)) // Start tracking or update tracking of active quest
	{
		const auto& Quest = PC->GetQuestManager()->ActiveQuests[QuestID];

		// Get objective stats
		FObjectiveProgress ObjProg;
		
		auto CurrObj = Quest->QuestInfo.CurrentObjective;

		ObjProg.ObjectiveID = Quest->Objectives[CurrObj].OptionalObjIndex;

		if (ObjProg.ObjectiveID != UTCStatics::DEFAULT_OBJECTIVE_ID)
		{
			ObjProg.ProgressGoal = Quest->OptionalObjectives[ObjProg.ObjectiveID].ProgressGoal;

			// Track it in our map
			OptObjProgress.Emplace(QuestID, ObjProg);
		}
		return true;
	}
	else if (OptObjProgress.Contains(QuestID)) // Stop tracking progress for completed/failed quests
	{
		OptObjProgress.Remove(QuestID);

		return true;
	}
	else
		return false;
}