// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Components/ObjectiveComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Game/QuestManager.h"
#include "Player/PlayerControllerBase.h"

// Sets default values for this component's properties
UObjectiveComponent::UObjectiveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
}

void UObjectiveComponent::ChangeCurrentQuest(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		// If we successfully update quest and aren't currently tracking it...
		if (PC->SetCurrentQuest(QuestID) && !ObjProgress.Contains(QuestID))
		{
			const auto& Quest = PC->GetQuestManager()->ActiveQuests[QuestID];

			// Get objective stats
			FObjectiveProgress ObjProg;
			ObjProg.ObjectiveID = Quest->QuestInfo.CurrentObjective;
			ObjProg.ProgressGoal = Quest->Objectives[ObjProg.ObjectiveID].ProgressGoal;

			// Track it in our map
			ObjProgress.Emplace(QuestID, ObjProg);
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
		return (PC->GetQuestManager()->BeginQuest(QuestID, MakeActive));
	}
	else
		return false;
}

// WIP
bool UObjectiveComponent::ProgressQuest(int32 QuestID, bool CurrCompleted)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		return (PC->GetQuestManager()->AdvanceQuest(QuestID, CurrCompleted));
	}
	else
		return false;
}

// WIP
bool UObjectiveComponent::ProgressObjective(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		return (PC->GetQuestManager()->AdvanceQuest(QuestID, true));
	}
	else
		return false;
}

// WIP
bool UObjectiveComponent::ProgressOptionalObjective(int32 QuestID)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		return (PC->GetQuestManager()->FinishOptionalObjective(QuestID, true));
	}
	else
		return false;
}

bool UObjectiveComponent::FinishQuest(int32 QuestID, bool Completed)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		if (Completed)
			return PC->GetQuestManager()->CompleteQuest(QuestID);
		else
			return PC->GetQuestManager()->FailQuest(QuestID);
	}
	else
		return false;
}