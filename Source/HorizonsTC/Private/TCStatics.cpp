// Copyright 2020 Jack Vento. All Rights Reserved.


#include "TCStatics.h"

// Quests
int32 UTCStatics::DEFAULT_QUEST_ID = 0;

int32 UTCStatics::DEFAULT_OBJECTIVE_ID = -1;


// Footsteps
FName UTCStatics::FOOTSTEPS_PLAY_PARAM = TEXT("PlayFootsteps");

FName UTCStatics::FOOTSTEPS_VOL_PARAM = TEXT("FootstepVolume");

FName UTCStatics::FOOTSTEPS_FLOOR_PARAM = TEXT("FloorMaterial");

FName UTCStatics::FOOTSTEPS_TYPE_PARAM = TEXT("FootstepsType");

int32 UTCStatics::DEFAULT_FOOTSTEP_INDEX = 1; // 1 = Concrete

FName UTCStatics::FOOTSTEPS_CURVE_NAME = TEXT("Mask_FootstepSound");

// Paths
FString UTCStatics::WEAPON_DB_PATH = TEXT("DataTable'/Game/HorizonsTC/Blueprints/Data/DataTables/DT_WeaponsDB.DT_WeaponsDB'");