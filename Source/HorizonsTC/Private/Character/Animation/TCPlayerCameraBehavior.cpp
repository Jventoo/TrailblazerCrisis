// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/Animation/TCPlayerCameraBehavior.h"


#include "Character/TCBaseCharacter.h"

void UTCPlayerCameraBehavior::NativeUpdateAnimation(float DeltaSeconds)
{
	if (ControlledPawn)
	{
		MovementState = ControlledPawn->GetMovementState();
		MovementAction = ControlledPawn->GetMovementAction();
		RotationMode = ControlledPawn->GetRotationMode();
		Gait = ControlledPawn->GetGait();
		Stance = ControlledPawn->GetStance();
		ViewMode = ControlledPawn->GetViewMode();
		bRightShoulder = ControlledPawn->IsRightShoulder();
	}
}
