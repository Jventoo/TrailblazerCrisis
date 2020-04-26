// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCStatics.h"
#include "ObjectiveComponent.generated.h"

USTRUCT()
struct FObjectiveProgress
{
	GENERATED_USTRUCT_BODY()

	FObjectiveProgress()
	{
		ObjectiveID = UTCStatics::DEFAULT_OBJECTIVE_ID;
		CurrentProgress = 0;
		ProgressGoal = 0;
	}

	void IncrementProgress()
	{
		++CurrentProgress;
	}

	int32 ObjectiveID;

	int32 CurrentProgress;

	int32 ProgressGoal;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAILBLAZERCRISIS_API UObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectiveComponent();

private:
	// Tracks progress on all current objectives. Completed objectives are removed from map
	UPROPERTY()
	TMap<int32, FObjectiveProgress> ObjProgress;

	// Tracks progress on all current optional objectives. Completed objectives are removed from map
	UPROPERTY()
		TMap<int32, FObjectiveProgress> OptObjProgress;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ChangeCurrentQuest(int32 QuestID);

	int32 GetCurrentQuest() const;

	bool BeginQuest(int32 QuestID, bool MakeActive);

	// Move to next objective and potentially finish the quest
	bool ProgressQuest(int32 QuestID, bool CurrCompleted);

	// Make progress on the current objective until it hits the progress goal
	bool ProgressObjective(int32 QuestID);

	bool ProgressOptionalObjective(int32 QuestID);

	bool FinishQuest(int32 QuestID, bool Completed);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
