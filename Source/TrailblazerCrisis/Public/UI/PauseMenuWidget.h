// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

UENUM()
enum class EPauseMenuTypes : uint8
{
	Pause,
	Quest,
	Journal,
	Options
};

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPauseMenuWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = UI)
		void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleQuestMenu();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleJournal();

	UFUNCTION(BlueprintCallable, Category = UI)
		void ToggleOptionsMenu();


	void InitPauseMenu(EPauseMenuTypes Type);

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
