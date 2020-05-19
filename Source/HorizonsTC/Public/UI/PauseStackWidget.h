// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseStackWidget.generated.h"


UENUM(BlueprintType)
enum class EPauseMenuTypes : uint8
{
	NONE,
	Pause,
	Quest,
	Journal,
	Options
};


/**
 * 
 */
UCLASS()
class HORIZONSTC_API UPauseStackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPauseStackWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = UI)
		void OpenMenu(EPauseMenuTypes Type, bool CloseOthers = false);

	UFUNCTION(BlueprintCallable, Category = UI)
		void CloseMenu(EPauseMenuTypes Type);

	UFUNCTION(BlueprintCallable, Category = UI)
		void TogglePauseMenu(bool Open);

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleQuestMenu(bool Open);

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleJournal(bool Open);

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleOptionsMenu(bool Open);

	UFUNCTION(BlueprintCallable, Category = UI)
		void DestroyChildMenus();

	UFUNCTION(BlueprintCallable, Category = UI)
		void GoToMainMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ResumeGameplay();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsPauseMenuOpen() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsQuestMenuOpen() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsJournalOpen() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		bool IsOptionsMenuOpen() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UI)
		EPauseMenuTypes GetOpenMenu() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (BindWidget))
		UUserWidget* PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (BindWidget))
		UUserWidget* QuestMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (BindWidget))
		UUserWidget* OptionsMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (BindWidget))
		UUserWidget* JournalMenu;

private:

	bool bPauseOpen;

	bool bQuestOpen;

	bool bJournalOpen;

	bool bOptionsOpen;
};
