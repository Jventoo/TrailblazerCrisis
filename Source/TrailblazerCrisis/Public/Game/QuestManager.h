// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Quests/MasterQuest.h"
#include "QuestManager.generated.h"

UCLASS()
class TRAILBLAZERCRISIS_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestManager();

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TArray<int32> CompletedQuests;

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TArray<int32> FailedQuests;

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TMap<int32, AMasterQuest*> ActiveQuests;

	UPROPERTY(BlueprintReadWrite, Category = Quests)
		TMap<int32, TSubclassOf<AMasterQuest>> AllQuests;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool BeginQuest(int32 QuestID, bool MakeActive);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool AdvanceQuest(int32 QuestID, bool CurrObjCompleted);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool FinishObjective(int32 QuestID, bool ObjCompleted);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool CompleteQuest(int32 QuestID);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool FailQuest(int32 QuestID);

};
