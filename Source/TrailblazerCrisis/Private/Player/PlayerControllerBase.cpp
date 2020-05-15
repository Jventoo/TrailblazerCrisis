// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Player/PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Actors/Components/ObjectiveComponent.h"
#include "TCStatics.h"
#include "UI/HUDWidget.h"
#include "UI/PauseStackWidget.h"
#include "Game/TC_PlayerCameraManager.h"

#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

APlayerControllerBase::APlayerControllerBase()
{
	HUDRef = nullptr;
	PauseStackRef = nullptr;
	
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

	/*if (PlayerCameraManager)
	{
		auto TCCameraManager = Cast<ATC_PlayerCameraManager>(PlayerCameraManager);

		ACharacter* CharacterPtr = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		APawn* PawnPtr = Cast<APawn>(CharacterPtr);

		if (TCCameraManager && PawnPtr)
			TCCameraManager->OnPossess(PawnPtr);
	}*/
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto& QuestAction = InputComponent->BindAction("QuestMenu", IE_Pressed, this, &APlayerControllerBase::ToggleQuestMenu);
	QuestAction.bExecuteWhenPaused = true;
	//QuestAction.bConsumeInput = true;

	auto& PauseAction = InputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayerControllerBase::TogglePauseMenu);
	PauseAction.bExecuteWhenPaused = true;
	//PauseAction.bConsumeInput = true;
}

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (PlayerCameraManager)
	{
		auto TCCameraManager = Cast<ATC_PlayerCameraManager>(PlayerCameraManager);

		if (TCCameraManager)
			TCCameraManager->OnPossess(aPawn);
	}
}

bool APlayerControllerBase::IsHudOpen() const
{
	return bHUDOpen;
}

bool APlayerControllerBase::IsPauseStackOpen() const
{
	return bUsingPauseMenus;
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

void APlayerControllerBase::TransitionToUI(bool CloseHUD)
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

void APlayerControllerBase::TransitionToGameplay(bool OpenHUD)
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

void APlayerControllerBase::ToggleQuestMenu()
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

void APlayerControllerBase::TogglePauseMenu()
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


void APlayerControllerBase::ToggleCrosshair(bool bEnabled)
{
	if (HUDRef)
	{
		UHUDWidget* HUD = Cast<UHUDWidget>(HUDRef);
		
		HUD->SetCrosshairEnabled(bEnabled);
	}
}