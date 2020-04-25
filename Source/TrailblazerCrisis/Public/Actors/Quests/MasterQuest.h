// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCStatics.h"
#include "MasterQuest.generated.h"

UENUM(BlueprintType)
enum class EObjectiveTypes : uint8
{
	Interact		UMETA(DisplayName = "Interact"),
	Attack			UMETA(DisplayName = "Attack"),
	Travel			UMETA(DisplayName = "Travel"),
	Build			UMETA(DisplayName = "Build")
};


USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_USTRUCT_BODY()

	FObjectiveData()
	{
		Title = FText::FromString("ObjTitle");

		Description = FText::FromString("ObjDesc");

		Type = EObjectiveTypes::Interact;

		CurrentProgress = 0;

		ProgressGoal = 0;

		OptionalObjIndex = UTCStatics::DEFAULT_OBJECTIVE_ID;

		IsOptional = false;

		Completed = false;

		NewQuestOnComplete = UTCStatics::DEFAULT_QUEST_ID;
	}

	UPROPERTY(BlueprintReadWrite)
		FText Title;

	UPROPERTY(BlueprintReadWrite)
		FText Description;

	UPROPERTY(BlueprintReadWrite)
		EObjectiveTypes Type;

	UPROPERTY(BlueprintReadWrite)
		int32 CurrentProgress;

	UPROPERTY(BlueprintReadWrite)
		int32 ProgressGoal;

	UPROPERTY(BlueprintReadWrite)
		int32 OptionalObjIndex;

	UPROPERTY(BlueprintReadWrite)
		bool IsOptional;

	UPROPERTY(BlueprintReadWrite)
		bool Completed;

	UPROPERTY(BlueprintReadWrite)
		int32 NewQuestOnComplete;

};


USTRUCT(BlueprintType)
struct FQuestStruct
{
	GENERATED_USTRUCT_BODY()

	FQuestStruct()
	{
		QuestName = FText::FromString("QuestName");

		QuestDesc = FText::FromString("QuestDesc");

		QuestID = UTCStatics::DEFAULT_QUEST_ID;

		FollowUpQuest = UTCStatics::DEFAULT_QUEST_ID;

		FailFollowUpQuest = UTCStatics::DEFAULT_QUEST_ID;

		CurrentObjective = UTCStatics::DEFAULT_OBJECTIVE_ID;
	}

	void IncrementCurrentObjective()
	{
		CurrentObjective += 1;
	}

	UPROPERTY(BlueprintReadWrite)
		FText QuestName;

	UPROPERTY(BlueprintReadWrite)
		FText QuestDesc;

	UPROPERTY(BlueprintReadWrite)
		int32 QuestID;

	UPROPERTY(BlueprintReadWrite)
		int32 FollowUpQuest;

	UPROPERTY(BlueprintReadWrite)
		int32 FailFollowUpQuest;

	UPROPERTY(BlueprintReadWrite)
		int32 CurrentObjective;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFailDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FObjectiveAdvanceDelegate, int32, OldObj, int32, NewObj);

UCLASS()
class TRAILBLAZERCRISIS_API AMasterQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterQuest();

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		FQuestStruct QuestInfo;

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TArray<FObjectiveData> Objectives;

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TArray<FObjectiveData> OptionalObjectives;

	UPROPERTY(BlueprintAssignable, Category = Quests)
		FBeginDelegate OnBeginDelegate;

	UPROPERTY(BlueprintAssignable, Category = Quests)
		FFailDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, Category = Quests)
		FCompletedDelegate OnCompletedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Quests)
		FObjectiveAdvanceDelegate OnObjectiveAdvanceDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
