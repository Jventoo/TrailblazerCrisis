// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Game/QuestManager.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	APlayerControllerBase();

	///////////////////////////////////////////////////
	/// Quests

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Quests)
		AQuestManager* GetQuestManager() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Quests)
		int32 GetCurrentQuest() const;

	// Not to be used directly. Should use wrapper in ObjectiveComponent
	UFUNCTION(BlueprintCallable, Category = Quests)
		bool SetCurrentQuest(int32 NewQuest, bool Bypass = false);

	///////////////////////////////////////////////////
	/// UI

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsHudOpen() const;

	UFUNCTION(BlueprintCallable, Category = UI)
		bool TransitionToUI(TSubclassOf<UUserWidget> UIClass, 
			UUserWidget* &MenuToOpen, bool CloseHUD = true);

	UFUNCTION(BlueprintCallable, Category = UI)
		bool TransitionToGameplay(UUserWidget* &MenuToClose, bool OpenHUD = true);

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleQuestMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void UpdateQuestHUD(int32 QuestID);

	UFUNCTION(BlueprintCallable, Category = UI)
		void TogglePauseMenu();

	///////////////////////////////////////////////////
	/// Components

	/** Returns ObjectiveComp subobject **/
	FORCEINLINE class UObjectiveComponent* GetObjectiveComp() const { return ObjectiveComp; }

	// Potentially change to HUDWidget class to avoid unnecessary casting
	FORCEINLINE class UUserWidget* GetHUDRef() { return HUDRef; }

	// Potentially change to PauseMenuWidget class to avoid unnecessary casting
	FORCEINLINE class UUserWidget* GetPauseRef() { return PauseUIRef; }

	///////////////////////////////////////////////////
	/// Utility

	UFUNCTION(BlueprintCallable, Category = Utility)
		void TransitionToMainMenu();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	
	///////////////////////////////////////////////////
	/// Quests

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quests)
		TSubclassOf<AQuestManager> QuestManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = Quests)
		AQuestManager* QuestManagerRef;

	int32 CurrentQuestID;

	///////////////////////////////////////////////////
	/// UI

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UUserWidget> HUDUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UUserWidget> PauseUIClass;

	UPROPERTY(BlueprintReadOnly, Category = UI)
		UUserWidget* HUDRef;

	UPROPERTY(BlueprintReadOnly, Category = UI)
		UUserWidget* PauseUIRef;

	bool bHUDOpen;

	bool bUsingPauseMenus;

	///////////////////////////////////////////////////
	/// Utility

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		FName MainMenuLevel;

private:
	/** Communication w quest manager and updates objective progress */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interactions, meta = (AllowPrivateAccess = "true"))
		class UObjectiveComponent* ObjectiveComp;
};
