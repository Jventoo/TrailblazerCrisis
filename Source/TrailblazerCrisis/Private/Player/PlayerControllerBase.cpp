// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Player/PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

APlayerControllerBase::APlayerControllerBase()
{
	HUDRef = nullptr;
	QuestUIRef = nullptr;
	
	OldQuestID = 0;
	CurrentQuestID = 0;

	bHUDOpen = false;
	bQuestOpen = false;
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
}

bool APlayerControllerBase::IsHudOpen() const
{
	return bHUDOpen;
}

bool APlayerControllerBase::IsQuestMenuOpen() const
{
	return bQuestOpen;
}

AQuestManager* APlayerControllerBase::GetQuestManager() const
{
	return QuestManagerRef;
}

int32 APlayerControllerBase::GetCurrentQuest() const
{
	return CurrentQuestID;
}

bool APlayerControllerBase::SetCurrentQuest(int32 NewCurrentQuestID)
{
	// Only update current quest if we actually have the quest
	if (QuestManagerRef && QuestManagerRef->ActiveQuests.Contains(NewCurrentQuestID))
	{
		CurrentQuestID = NewCurrentQuestID;
		return true;
	}
	else
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
	if (bQuestOpen)
	{
		if (TransitionToGameplay(QuestUIRef))
		{
			bQuestOpen = false;

			if (OldQuestID != CurrentQuestID)
				bQuestOpen = false; // <- Placeholder for future event call to update UI
		}
	}
	else
	{
		if (TransitionToUI(QuestUIClass, QuestUIRef))
		{
			bQuestOpen = true;
			OldQuestID = CurrentQuestID;
		}
	}
}