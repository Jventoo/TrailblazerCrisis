// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "TCStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"


FTransformAndComp UTCStatics::ComponentLocalToWorld(const FTransformAndComp& LocalSpace)
{
	return FTransformAndComp(LocalSpace.Transform * LocalSpace.Comp->GetComponentTransform(), LocalSpace.Comp);
}

FTransformAndComp UTCStatics::ComponentWorldToLocal(const FTransformAndComp& WorldSpace)
{
	return FTransformAndComp(WorldSpace.Transform * UKismetMathLibrary::InvertTransform(
		WorldSpace.Comp->GetComponentTransform()), WorldSpace.Comp);
}

FTransform UTCStatics::AddTransforms(const FTransform& A, const FTransform& B)
{
	return FTransform(FRotator(A.Rotator().Pitch + B.Rotator().Pitch, A.Rotator().Yaw + B.Rotator().Yaw, A.Rotator().Roll + B.Rotator().Roll),
		A.GetLocation() + B.GetLocation());
}

FTransform UTCStatics::SubTransforms(const FTransform& A, const FTransform& B)
{
	return FTransform(FRotator(A.Rotator().Pitch - B.Rotator().Pitch, A.Rotator().Yaw - B.Rotator().Yaw, A.Rotator().Roll - B.Rotator().Roll),
		A.GetLocation() - B.GetLocation());
}

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

FName UTCStatics::FOOTSTEPS_TYPE_PARAM = TEXT("FootstepsType");

int32 UTCStatics::DEFAULT_FOOTSTEP_INDEX = 1; // 1 = Concrete

FName UTCStatics::FOOTSTEPS_CURVE_NAME = TEXT("Mask_FootstepSound");

// Camera
float UTCStatics::DEFAULT_TP_TRACE_RADIUS = 10.0f;

FName UTCStatics::FP_CAMERA_SOCKET = TEXT("FP_Camera");

FName UTCStatics::CAMERA_MANAGER_ROT_CURVE = TEXT("RotationLagSpeed");

FName UTCStatics::CAMERA_MANAGER_DEBUG_CURVE = TEXT("Override_Debug");