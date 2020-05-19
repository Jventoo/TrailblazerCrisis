// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Quests/MasterQuest.h"
#include "QuestManager.generated.h"

UCLASS()
class HORIZONSTC_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Quests)
		TArray<int32> CompletedQuests;

	// <Key: QuestID, Value: FailedObjID>
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Quests)
		TMap<int32, int32> FailedQuests;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Quests)
		TMap<int32, AMasterQuest*> ActiveQuests;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Quests)
		TMap<int32, TSubclassOf<AMasterQuest>> AllQuests;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool BeginQuest(int32 QuestID, bool MakeActive);

	// Finish our current objective and move to the next
	UFUNCTION(BlueprintCallable, Category = Quests)
		bool AdvanceQuest(int32 QuestID, bool CurrObjCompleted);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool FinishOptionalObjective(int32 QuestID, bool ObjCompleted);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool CompleteQuest(int32 QuestID);

	UFUNCTION(BlueprintCallable, Category = Quests)
		bool FailQuest(int32 QuestID);

};
