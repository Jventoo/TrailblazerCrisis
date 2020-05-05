// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "TCStatics.h"

// Movement
float UTCStatics::MAX_MOVE_SPEED = 1.0f;

float UTCStatics::SPRINT_MODIFIER = 1.5f;

// Quests
int32 UTCStatics::DEFAULT_QUEST_ID = 0;

int32 UTCStatics::DEFAULT_OBJECTIVE_ID = -1;


// Footsteps
FName UTCStatics::FOOTSTEPS_PLAY_PARAM = TEXT("PlayFootsteps");

FName UTCStatics::FOOTSTEPS_VOL_PARAM = TEXT("FootstepVolume");

FName UTCStatics::FOOTSTEPS_FLOOR_PARAM = TEXT("FloorMaterial");

int32 UTCStatics::DEFAULT_FOOTSTEP_INDEX = 1; // 1 = Concrete