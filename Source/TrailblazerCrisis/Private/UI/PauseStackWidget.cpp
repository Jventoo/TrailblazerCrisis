// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "UI/PauseStackWidget.h"
#include "Player/PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"

UPauseStackWidget::UPauseStackWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bQuestOpen   = false;
	bJournalOpen = false;
	bOptionsOpen = false;
	bPauseOpen	 = false;
}

void UPauseStackWidget::OpenMenu(EPauseMenuTypes Type, bool CloseOthers)
{
	switch (Type)
	{

	case EPauseMenuTypes::Pause:

		if (CloseOthers)
		{
			ToggleQuestMenu(false);
			ToggleOptionsMenu(false);
			ToggleJournal(false);
		}
		
		TogglePauseMenu(true);

		break;

	case EPauseMenuTypes::Quest:

		if (CloseOthers)
		{
			TogglePauseMenu(false);
			ToggleOptionsMenu(false);
			ToggleJournal(false);
		}

		ToggleQuestMenu(true);

		break;

	case EPauseMenuTypes::Journal:

		if (CloseOthers)
		{
			TogglePauseMenu(false);
			ToggleQuestMenu(false);
			ToggleOptionsMenu(false);
		}

		ToggleJournal(true);

		break;

	case EPauseMenuTypes::Options:

		if (CloseOthers)
		{
			TogglePauseMenu(false);
			ToggleQuestMenu(false);
			ToggleJournal(false);
		}

		ToggleOptionsMenu(true);

		break;

	default:
		break;
	}
}

void UPauseStackWidget::CloseMenu(EPauseMenuTypes Type)
{
	switch (Type)
	{

	case EPauseMenuTypes::Pause:

		TogglePauseMenu(false);

		break;

	case EPauseMenuTypes::Quest:

		ToggleQuestMenu(false);

		break;

	case EPauseMenuTypes::Journal:

		ToggleJournal(false);

		break;

	case EPauseMenuTypes::Options:

		ToggleOptionsMenu(false);

		break;

	default:
		break;
	}
}

void UPauseStackWidget::TogglePauseMenu(bool Open)
{
	if (!Open)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
		bPauseOpen = false;
	}
	else
	{
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		bPauseOpen = true;
	}
}

void UPauseStackWidget::ToggleQuestMenu(bool Open)
{
	if (!Open)
	{
		QuestMenu->SetVisibility(ESlateVisibility::Collapsed);
		bQuestOpen = false;
	}
	else
	{
		QuestMenu->SetVisibility(ESlateVisibility::Visible);
		bQuestOpen = true;
	}
}

void UPauseStackWidget::ToggleJournal(bool Open)
{
	if (!Open)
	{
		JournalMenu->SetVisibility(ESlateVisibility::Collapsed);
		bJournalOpen = false;
	}
	else
	{
		JournalMenu->SetVisibility(ESlateVisibility::Visible);
		bJournalOpen = true;
	}
}

void UPauseStackWidget::ToggleOptionsMenu(bool Open)
{
	if (!Open)
	{
		OptionsMenu->SetVisibility(ESlateVisibility::Collapsed);
		bOptionsOpen = false;
	}
	else
	{
		OptionsMenu->SetVisibility(ESlateVisibility::Visible);
		bOptionsOpen = true;
	}
}

void UPauseStackWidget::DestroyChildMenus()
{
	bQuestOpen = false;
	bJournalOpen = false;
	bOptionsOpen = false;
	bQuestOpen = false;

	PauseMenu->RemoveFromParent();
	PauseMenu = nullptr;

	QuestMenu->RemoveFromParent();
	QuestMenu = nullptr;

	JournalMenu->RemoveFromParent();
	JournalMenu = nullptr;

	OptionsMenu->RemoveFromParent();
	OptionsMenu = nullptr;
}

void UPauseStackWidget::GoToMainMenu()
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
		PC->TransitionToMainMenu();
}

void UPauseStackWidget::ResumeGameplay()
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		DestroyChildMenus();

		PC->TransitionToGameplay();
	}
}

bool UPauseStackWidget::IsPauseMenuOpen() const
{
	return bPauseOpen;
}

bool UPauseStackWidget::IsQuestMenuOpen() const
{
	return bQuestOpen;
}

bool UPauseStackWidget::IsJournalOpen() const
{
	return bJournalOpen;
}

bool UPauseStackWidget::IsOptionsMenuOpen() const
{
	return bOptionsOpen;
}

EPauseMenuTypes UPauseStackWidget::GetOpenMenu() const
{
	if (bPauseOpen)
		return EPauseMenuTypes::Pause;
	else if (bQuestOpen)
		return EPauseMenuTypes::Quest;
	else if (bJournalOpen)
		return EPauseMenuTypes::Journal;
	else if (bOptionsOpen)
		return EPauseMenuTypes::Options;
	else
		return EPauseMenuTypes::NONE;
}