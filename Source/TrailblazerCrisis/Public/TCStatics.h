// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCStatics.generated.h"

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking,
	Running,
	Sprinting
};

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None,
	LowMantle,
	HighMantle,
	Rolling,
	GettingUp
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	None,
	Grounded,
	InAir,
	Mantling,
	Ragdoll
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default,
	Masculine,
	Feminine,
	Injured,
	HandsTied,
	Rifle,
	Pistol1H,
	Pistol2H,
	Bow,
	Torch,
	Binos,
	Box,
	Barrel
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection,
	LookingDirection,
	Aiming
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing,
	Crouching,
	Prone
};

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
	None,
	Roll
};


/**
 * 
 */
UCLASS(abstract)
class TRAILBLAZERCRISIS_API UTCStatics : public UObject
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* Movement																*/
	/************************************************************************/

	// Do not edit. Tied to root motion
	static float MAX_MOVE_SPEED;
	
	// Do not edit. Tied to root motion
	static float SPRINT_MODIFIER;
	

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
	/* Camera																*/
	/************************************************************************/

	static float DEFAULT_TP_TRACE_RADIUS;

	static FName FP_CAMERA_SOCKET;
};
