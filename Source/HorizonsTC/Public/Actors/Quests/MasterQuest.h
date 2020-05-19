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

		ProgressGoal = 0;

		OptionalObjIndex = UTCStatics::DEFAULT_OBJECTIVE_ID;

		IsFinished = false;

		Completed = false;

		NewQuestOnComplete = UTCStatics::DEFAULT_QUEST_ID;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		EObjectiveTypes Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 ProgressGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 OptionalObjIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		bool IsFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		bool Completed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
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

		CurrentObjective = 0;
	}

	void IncrementCurrentObjective()
	{
		++CurrentObjective;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		FText QuestDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 FollowUpQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 FailFollowUpQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Structs")
		int32 CurrentObjective;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFailDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FObjectiveAdvanceDelegate, int32, OldObj, int32, NewObj);

UCLASS()
class HORIZONSTC_API AMasterQuest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMasterQuest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quests)
		FQuestStruct QuestInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quests)
		TArray<FObjectiveData> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quests)
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
