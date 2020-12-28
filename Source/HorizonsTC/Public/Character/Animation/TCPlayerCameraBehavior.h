// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Library/TCCharacterEnumLibrary.h"

#include "TCPlayerCameraBehavior.generated.h"

class ATCBaseCharacter;
class ATCPlayerController;

/**
 * Main class for player camera movement behavior
 */
UCLASS(Blueprintable, BlueprintType)
class HORIZONSTC_API UTCPlayerCameraBehavior : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ATCBaseCharacter* ControlledPawn = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController* PlayerController = nullptr;

protected:
	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMovementAction MovementAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ERotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EViewMode ViewMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bRightShoulder;
};
