// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/TCPlayerController.h"
#include "Character/TCCharacter.h"
#include "Character/TCPlayerCameraManager.h"
#include "Actors/Quests/QuestManager.h"
#include "TCStatics.h"

#include "UI/HUDWidget.h"
#include "UI/PauseStackWidget.h"
#include "UI/HUDWidget.h"
#include "UI/PauseStackWidget.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Components/ObjectiveComponent.h"
#include "Actors/Weapons/BaseFirearm.h"

ATCPlayerController::ATCPlayerController()
{
	CurrentQuestID = UTCStatics::DEFAULT_QUEST_ID;

	bHUDOpen = false;
	bUsingPauseMenus = false;

	// Create objective component to communicate with quest manager
	ObjectiveComp = CreateDefaultSubobject<UObjectiveComponent>(TEXT("Objective"));
}

void ATCPlayerController::BeginPlay()
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

void ATCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto& QuestAction = InputComponent->BindAction("QuestMenu", IE_Pressed, this, &ATCPlayerController::ToggleQuestMenu);
	QuestAction.bExecuteWhenPaused = true;
	QuestAction.bConsumeInput = true;

	auto& PauseAction = InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ATCPlayerController::TogglePauseMenu);
	PauseAction.bExecuteWhenPaused = true;
	PauseAction.bConsumeInput = true;
}

bool ATCPlayerController::IsHudOpen() const
{
	return bHUDOpen;
}

bool ATCPlayerController::IsPauseStackOpen() const
{
	return bUsingPauseMenus;
}

AQuestManager* ATCPlayerController::GetQuestManager() const
{
	return QuestManagerRef;
}

int32 ATCPlayerController::GetCurrentQuest() const
{
	return CurrentQuestID;
}

bool ATCPlayerController::SetCurrentQuest(int32 NewCurrentQuestID, bool Bypass)
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

void ATCPlayerController::TransitionToUI(bool CloseHUD)
{
	// Collapse HUD
	if (CloseHUD && HUDRef)
	{
		HUDRef->SetVisibility(ESlateVisibility::Collapsed);
		bHUDOpen = false;
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ATCPlayerController::TransitionToGameplay(bool OpenHUD)
{
	// Open HUD
	if (OpenHUD && HUDRef)
	{
		HUDRef->SetVisibility(ESlateVisibility::Visible);
		bHUDOpen = true;
	}

	if (PauseStackRef)
	{
		bUsingPauseMenus = false;

		PauseStackRef->RemoveFromParent();
		PauseStackRef = nullptr;
	}

	MenuHistoryStack.Empty();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ATCPlayerController::ToggleQuestMenu()
{
	// 3 Cases: Call from gameplay, call from other pause menu stack child, call from pause menu

	if (bUsingPauseMenus)
	{
		UPauseStackWidget* PauseStack = Cast<UPauseStackWidget>(PauseStackRef);
		if (PauseStack)
		{
			if (PauseStack->IsQuestMenuOpen())
			{
				// Case 3: Call from quest menu
				PauseStack->ResumeGameplay();

				bUsingPauseMenus = false;
			}
			else
			{
				// Case 2: Call from other pause stack menu, open quest menu
				PauseStack->CloseMenu(PauseStack->GetOpenMenu());
				PauseStack->OpenMenu(EPauseMenuTypes::Quest, false);

				bUsingPauseMenus = true;
			}
		}
	}
	else
	{
		// Case 1: From Gameplay
		if (PauseStackClass)
		{
			// Create the master widget and add it to the viewport
			PauseStackRef = CreateWidget<UUserWidget>(GetWorld(), PauseStackClass);

			if (PauseStackRef)
			{
				PauseStackRef->AddToViewport();

				// Tell the quest menu to open
				UPauseStackWidget* PauseStack = Cast<UPauseStackWidget>(PauseStackRef);
				if (PauseStack)
				{
					TransitionToUI();

					PauseStack->OpenMenu(EPauseMenuTypes::Quest, false);

					bUsingPauseMenus = true;
				}
			}
		}
	}
}

void ATCPlayerController::TogglePauseMenu()
{
	// 3 Cases: Call from gameplay, call from other pause menu stack child, call from pause menu

	if (bUsingPauseMenus)
	{
		UPauseStackWidget* PauseStack = Cast<UPauseStackWidget>(PauseStackRef);
		if (PauseStack)
		{
			if (PauseStack->IsPauseMenuOpen())
			{
				// Case 3: Call from pause menu
				PauseStack->ResumeGameplay();

				bUsingPauseMenus = false;
			}
			else
			{
				// Case 2: Call from other pause stack menu, return to pause menu
				PauseStack->CloseMenu(PauseStack->GetOpenMenu());
				PauseStack->OpenMenu(EPauseMenuTypes::Pause, false);

				bUsingPauseMenus = true;
			}
		}
	}
	else
	{
		// Case 1: From Gameplay
		if (PauseStackClass)
		{
			// Create the master widget and add it to the viewport
			PauseStackRef = CreateWidget<UUserWidget>(GetWorld(), PauseStackClass);

			if (PauseStackRef)
			{
				PauseStackRef->AddToViewport();

				// Tell the pause menu to open
				UPauseStackWidget* PauseStack = Cast<UPauseStackWidget>(PauseStackRef);
				if (PauseStack)
				{
					TransitionToUI();

					PauseStack->OpenMenu(EPauseMenuTypes::Pause, false);

					bUsingPauseMenus = true;
				}
			}
		}
	}
}


void ATCPlayerController::UpdateQuestHUD(int32 QuestID)
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


void ATCPlayerController::TransitionToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevel);
}


void ATCPlayerController::ToggleCrosshair(bool bEnabled)
{
	if (HUDRef)
	{
		UHUDWidget* HUD = Cast<UHUDWidget>(HUDRef);

		HUD->SetCrosshairEnabled(bEnabled);
	}
}

void ATCPlayerController::OnRestartPawn(APawn* NewPawn)
{
	PossessedCharacter = Cast<ATCCharacter>(NewPawn);
	check(PossessedCharacter);

	// Call "OnPossess" in Player Camera Manager when possessing a pawn
	auto CastedMgr = Cast<ATCPlayerCameraManager>(PlayerCameraManager);
	if (CastedMgr)
	{
		CastedMgr->OnPossess(PossessedCharacter);
	}
}

void ATCPlayerController::SetLimitedInputMode(bool LimitedInput, bool SetFirstPerson, bool HideHUD, bool HideWeapon)
{
	// Optionally hide the HUD
	if (HideHUD && HUDRef)
	{
		HUDRef->SetVisibility(ESlateVisibility::Collapsed);
		bHUDOpen = false;
	}

	auto PlayerChar = Cast<ATCCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerChar)
	{
		// Set view mode based on param
		PlayerChar->SetViewMode(SetFirstPerson ? EViewMode::FirstPerson : EViewMode::ThirdPerson);
		
		// Set jump jets enabled
		PlayerChar->SetJumpJetsEnabled(!LimitedInput);

		// Optionally unequip current firearm
		if (HideWeapon)
		{
			auto CurrWep = PlayerChar->GetCurrentWeapon(); 
			
			if (CurrWep && CurrWep->IsEquipped())
				PlayerChar->ToggleEquip();
		}

		PlayerChar->SetSprintDisabled(LimitedInput);
	}

	bLimitedInputMode = LimitedInput;
}

bool ATCPlayerController::IsInLimitedInputMode() const
{
	return bLimitedInputMode;
}
