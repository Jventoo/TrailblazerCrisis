// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCStatics.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UTCStatics : public UObject
{
	GENERATED_BODY()
	
public:
	// Do not edit. Tied to root motion
	static float MAX_MOVE_SPEED;
	
	// Do not edit. Tied to root motion
	static float SPRINT_MODIFIER;

	// Editable. Default ID for new quests. Used to check whether a quest is valid. Do not lose sync with BP
	static uint32 DEFAULT_QUEST_ID;

	// Do not edit. Default ID for objectives. Used to check whether an objective is valid and essentially allow pointer behavior in BP
	static uint32 DEFAULT_OBJECTIVE_ID;
};
