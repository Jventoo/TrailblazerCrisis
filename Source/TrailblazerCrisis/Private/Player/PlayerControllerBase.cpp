// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Player/PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Actors/Components/ObjectiveComponent.h"
#include "TCStatics.h"
#include "UI/HUDWidget.h"
#include "UI/PauseMenuWidget.h"

APlayerControllerBase::APlayerControllerBase()
{
	HUDRef = nullptr;
	PauseUIRef = nullptr;
	
	CurrentQuestID = UTCStatics::DEFAULT_QUEST_ID;

	bHUDOpen = false;
	bUsingPauseMenus = false;

	// Create objective component to communicate with quest manager
	ObjectiveComp = CreateDefaultSubobject<UObjectiveComponent>(TEXT("Objective"));
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Quest Manager
	QuestManagerRef = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass);

	// Spawn HUD
	HUDRef = CreateWidget<UUserWidget>(GetWorld(), HUDUIClass);

	if (HUDRef)
	{
		HUDRef->AddToViewport();
		bHUDOpen = true;
	}
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto& QuestAction = InputComponent->BindAction("QuestMenu", IE_Pressed, this, &APlayerControllerBase::ToggleQuestMenu);
	QuestAction.bExecuteWhenPaused = true;
	QuestAction.bConsumeInput = true;

	auto& PauseAction = InputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayerControllerBase::TogglePauseMenu);
	PauseAction.bExecuteWhenPaused = true;
	PauseAction.bConsumeInput = true;
}

bool APlayerControllerBase::IsHudOpen() const
{
	return bHUDOpen;
}

AQuestManager* APlayerControllerBase::GetQuestManager() const
{
	return QuestManagerRef;
}

int32 APlayerControllerBase::GetCurrentQuest() const
{
	return CurrentQuestID;
}

bool APlayerControllerBase::SetCurrentQuest(int32 NewCurrentQuestID, bool Bypass)
{
	// Only update current quest if we actually have the quest
	if (NewCurrentQuestID != CurrentQuestID && (Bypass 
		|| (QuestManagerRef && QuestManagerRef->ActiveQuests.Contains(NewCurrentQuestID))))
	{
		CurrentQuestID = NewCurrentQuestID;
		UpdateQuestHUD(CurrentQuestID);

		return true;
	}
	
	return false;
}

bool APlayerControllerBase::TransitionToUI(
	TSubclassOf<UUserWidget> UIClass, UUserWidget* &MenuToOpen, bool CloseHUD)
{
	bool success = false;

	// Collapse HUD
	if (CloseHUD && HUDRef)
	{
		HUDRef->SetVisibility(ESlateVisibility::Collapsed);
		bHUDOpen = false;
	}

	// Delete reference to old menu in case we still have one
	if (MenuToOpen)
	{
		MenuToOpen->RemoveFromViewport();
		MenuToOpen = nullptr;
	}
	
	// Create menu and add to viewport
	if (UIClass)
	{
		MenuToOpen = CreateWidget<UUserWidget>(GetWorld(), UIClass);

		if (MenuToOpen)
		{
			MenuToOpen->AddToViewport();
			success = true;
		}
	}

	if (success)
	{
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	return success;
}

bool APlayerControllerBase::TransitionToGameplay(UUserWidget* &MenuToClose, bool OpenHUD)
{
	bool success = false;

	// Delete menu
	if (MenuToClose)
	{
		MenuToClose->RemoveFromViewport();
		MenuToClose = nullptr;
		success = true;
	}

	// Open HUD
	if (OpenHUD && HUDRef)
	{
		HUDRef->SetVisibility(ESlateVisibility::Visible);
		bHUDOpen = true;
	}

	if (success)
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	return success;
}

void APlayerControllerBase::ToggleQuestMenu()
{
	// Init pause menu if it wasn't already
	if (!PauseUIRef)
		TogglePauseMenu();

	// Tell pause menu to open quest menu
	UPauseMenuWidget* PauseMenu = Cast<UPauseMenuWidget>(PauseUIRef);
	if (PauseMenu)
	{
		PauseMenu->ToggleQuestMenu();
	}
}

void APlayerControllerBase::TogglePauseMenu()
{
	if (bUsingPauseMenus)
	{
		UPauseMenuWidget* PauseMenu = Cast<UPauseMenuWidget>(PauseUIRef);

		if (PauseMenu)
			PauseMenu->DestroyChildMenus();

		if (TransitionToGameplay(PauseUIRef))
			bUsingPauseMenus = false;
	}
	else
	{
		if (TransitionToUI(PauseUIClass, PauseUIRef))
		{
			UPauseMenuWidget* PauseMenu = Cast<UPauseMenuWidget>(PauseUIRef);

			bUsingPauseMenus = true;

			if (PauseMenu)
				PauseMenu->InitPauseMenu(EPauseMenuTypes::Pause);
		}
	}
}

void APlayerControllerBase::UpdateQuestHUD(int32 QuestID)
{
	UHUDWidget* HUD = Cast<UHUDWidget>(HUDRef);
	if (HUD)
	{
		if (QuestID != UTCStatics::DEFAULT_QUEST_ID)
		{
			const auto& Quest = QuestManagerRef->ActiveQuests[CurrentQuestID];
			const auto& Text = Quest->Objectives[Quest->QuestInfo.CurrentObjective].Description;

			HUD->UpdateQuestText(Text);
		}
		else
			HUD->UpdateQuestText(FText::FromString(""));
	}
}

void APlayerControllerBase::TransitionToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevel);
}