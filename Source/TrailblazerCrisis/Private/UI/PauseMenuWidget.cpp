// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "UI/PauseMenuWidget.h"
#include "Player/PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"

UPauseMenuWidget::UPauseMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bQuestOpen   = false;
	bJournalOpen = false;
	bOptionsOpen = false;
	bPauseOpen	 = false;
}

void UPauseMenuWidget::InitPauseMenu(EPauseMenuTypes Type)
{
	switch (Type)
	{

	case EPauseMenuTypes::Pause:

		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		bPauseOpen = true;
		break;

	case EPauseMenuTypes::Quest:

		QuestMenu->SetVisibility(ESlateVisibility::Visible);
		bQuestOpen = true;
		break;

	case EPauseMenuTypes::Journal:

		JournalMenu->SetVisibility(ESlateVisibility::Visible);
		bJournalOpen = true;
		break;

	case EPauseMenuTypes::Options:

		OptionsMenu->SetVisibility(ESlateVisibility::Visible);
		bOptionsOpen = true;
		break;

	}
}

void UPauseMenuWidget::TogglePauseMenu()
{
	if (bPauseOpen)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
		bPauseOpen = false;
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
		bPauseOpen = true;
	}
}

void UPauseMenuWidget::ToggleQuestMenu()
{
	if (bQuestOpen)
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

void UPauseMenuWidget::ToggleJournal()
{
	if (bJournalOpen)
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

void UPauseMenuWidget::ToggleOptionsMenu()
{
	if (bOptionsOpen)
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

void UPauseMenuWidget::DestroyChildMenus()
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

void UPauseMenuWidget::GoToMainMenu()
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
		PC->TransitionToMainMenu();
}

void UPauseMenuWidget::ResumeGameplay()
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		auto Ref = PC->GetPauseRef();
		PC->TransitionToGameplay(Ref);
	}
}

bool UPauseMenuWidget::IsPauseMenuOpen() const
{
	return bPauseOpen;
}

bool UPauseMenuWidget::IsQuestMenuOpen() const
{
	return bQuestOpen;
}

bool UPauseMenuWidget::IsJournalOpen() const
{
	return bJournalOpen;
}

bool UPauseMenuWidget::IsOptionsMenuOpen() const
{
	return bOptionsOpen;
}