// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "TCPlayerController.generated.h"

class ATCBaseCharacter;
class AQuestManager;

/**
* Player controller class
*/
UCLASS(Blueprintable, BlueprintType)
class HORIZONSTC_API ATCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATCPlayerController();

	/************************************************************************/
	/* Quests																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Quests)
		AQuestManager* GetQuestManager() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Quests)
		int32 GetCurrentQuest() const;

	// Not to be used directly. Should use wrapper in ObjectiveComponent
	UFUNCTION(BlueprintCallable, Category = Quests)
		bool SetCurrentQuest(int32 NewQuest, bool Bypass = false);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quests)
		TSubclassOf<AQuestManager> QuestManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = Quests)
		class AQuestManager* QuestManagerRef;

	int32 CurrentQuestID;


	/************************************************************************/
	/* UI																	*/
	/************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UUserWidget> HUDUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UUserWidget> PauseStackClass;

	UPROPERTY(BlueprintReadOnly, Category = UI)
		UUserWidget* HUDRef = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = UI)
		UUserWidget* PauseStackRef = nullptr;

	bool bHUDOpen = false;

	bool bUsingPauseMenus = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsHudOpen() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsPauseStackOpen() const;

	UFUNCTION(BlueprintCallable, Category = UI)
		void TransitionToUI(bool CloseHUD = true);

	UFUNCTION(BlueprintCallable, Category = UI)
		void TransitionToGameplay(bool OpenHUD = true);

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleQuestMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void UpdateQuestHUD(int32 QuestID);

	UFUNCTION(BlueprintCallable, Category = UI)
		void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleCrosshair(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
		bool IsCrosshairDisplayed() const;

	UPROPERTY(BlueprintReadOnly, Category = UI)
		TArray<UUserWidget*> MenuHistoryStack;


	/************************************************************************/
	/* Components															*/
	/************************************************************************/

	/** Returns ObjectiveComp subobject **/
	FORCEINLINE class UObjectiveComponent* GetObjectiveComp() const { return ObjectiveComp; }

	// Potentially change to HUDWidget class to avoid unnecessary casting
	FORCEINLINE class UUserWidget* GetHUDRef() { return HUDRef; }

	// Potentially change to PauseMenuWidget class to avoid unnecessary casting
	FORCEINLINE class UUserWidget* GetPauseStackRef() { return PauseStackRef; }


	/************************************************************************/
	/* Utility																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = Utility)
		void TransitionToMainMenu();

	void OnRestartPawn(APawn* NewPawn);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		FName MainMenuLevel;

	UFUNCTION(BlueprintCallable, Category = Utility)
		void SetLimitedInputMode(bool LimitedInput, bool SetFirstPerson = true, bool HideHUD = false, bool HideWeapon = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Utility)
		bool IsInLimitedInputMode() const;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	/** Main character reference */
	ATCBaseCharacter* PossessedCharacter = nullptr;

	/** Communication w quest manager and updates objective progress */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interactions, meta = (AllowPrivateAccess = "true"))
		class UObjectiveComponent* ObjectiveComp;

	bool bLimitedInputMode = false;
};
