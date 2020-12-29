// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCStatics.generated.h"

/**
 * 
 */
UCLASS(abstract)
class HORIZONSTC_API UTCStatics : public UObject
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* Quests																*/
	/************************************************************************/

	// Editable. Default ID for new quests. Used to check whether a quest is valid. Do not lose sync with BP
	static int32 DEFAULT_QUEST_ID;

	// Do not edit. Default ID for objectives. Used to check whether an objective is valid and essentially allow pointer behavior in BP
	static int32 DEFAULT_OBJECTIVE_ID;


	/************************************************************************/
	/* Footsteps															*/
	/************************************************************************/

	static FName FOOTSTEPS_PLAY_PARAM;

	static FName FOOTSTEPS_VOL_PARAM;

	static FName FOOTSTEPS_FLOOR_PARAM;

	static FName FOOTSTEPS_TYPE_PARAM;

	static int32 DEFAULT_FOOTSTEP_INDEX;

	static FName FOOTSTEPS_CURVE_NAME;

	/************************************************************************/
	/* Asset Paths															*/
	/************************************************************************/

	static FString WEAPON_DB_PATH;
};
