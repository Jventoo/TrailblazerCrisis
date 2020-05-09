// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Game/PlayerCameraBehavior.h"
#include "Interfaces/CameraInterface.h"
#include "Interfaces/CharacterInterface.h"

UPlayerCameraBehavior::UPlayerCameraBehavior()
{
	PlayerController = nullptr; 
	ControlledPawn = nullptr;

	bRightShoulder = false;
}

void UPlayerCameraBehavior::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateCharacterInfo();
}

void UPlayerCameraBehavior::UpdateCharacterInfo()
{
	if (ControlledPawn)
	{
		if (ControlledPawn->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			const auto& States = ICharacterInterface::Execute_GetCurrentStates(ControlledPawn);

			MovementState = States.MovementState;
			MovementAction = States.MovementAction;
			RotationMode = States.VelocityDir;
			Gait = States.ActualGait;
			Stance = States.ActualStance;
			ViewMode = States.ViewMode;
		}

		if (ControlledPawn->GetClass()->ImplementsInterface(UCameraInterface::StaticClass()))
		{
			float TP = 0.0f, FP = 0.0f;
			bRightShoulder = ICameraInterface::Execute_GetCameraParameters(
				ControlledPawn, TP, FP);
		}
	}
}