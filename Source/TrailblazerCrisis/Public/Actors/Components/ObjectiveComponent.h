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

	void IncrementProgress(int32 amount)
	{
		CurrentProgress += amount;
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

	UFUNCTION(BlueprintCallable, Category = "Quests")
		void ChangeCurrentQuest(int32 QuestID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
		int32 GetCurrentQuest() const;

	UFUNCTION(BlueprintCallable, Category = "Quests")
		bool BeginQuest(int32 QuestID, bool MakeActive);

	// Move to next objective and potentially finish the quest
	UFUNCTION(BlueprintCallable, Category = "Quests")
		bool ProgressQuest(int32 QuestID, bool CurrCompleted);

	// Make progress on the current objective until it hits the progress goal
	UFUNCTION(BlueprintCallable, Category = "Quests | Objectives")
		bool ProgressObjective(int32 QuestID, int32 ProgressIncrease);

	/**
	 * Add progress to the specified optional objective
	 * and potentially finish it if we hit our progress goal.
	 *
	 * @param 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "Quests | Objectives")
		bool ProgressOptionalObjective(int32 QuestID, int32 ProgressIncrease);

	UFUNCTION(BlueprintCallable, Category = "Quests")
		bool FinishQuest(int32 QuestID, bool Completed);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	bool UpdateObjectiveProgress(int32 QuestID);

	bool UpdateOptionalObjectiveProgress(int32 QuestID);

	int32 tempobj;

	int32 tempopt;
};
