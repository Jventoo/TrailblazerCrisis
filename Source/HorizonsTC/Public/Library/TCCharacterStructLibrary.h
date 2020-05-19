// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Library/TCCharacterEnumLibrary.h"

#include "TCCharacterStructLibrary.generated.h"

class UCurveVector;
class UAnimMontage;
class UAnimSequenceBase;
class UCurveFloat;

USTRUCT(BlueprintType)
struct FComponentAndTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	UPROPERTY(EditAnywhere)
	class UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float TargetArmLength = 0.0f;

	UPROPERTY(EditAnywhere)
	FVector SocketOffset;

	UPROPERTY(EditAnywhere)
	float LagSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	float RotationLagSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bDoCollisionTest = true;
};

USTRUCT(BlueprintType)
struct FCameraGaitSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FCameraSettings Walking;

	UPROPERTY(EditAnywhere)
	FCameraSettings Running;

	UPROPERTY(EditAnywhere)
	FCameraSettings Sprinting;

	UPROPERTY(EditAnywhere)
	FCameraSettings Crouching;
};

USTRUCT(BlueprintType)
struct FCameraStateSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FCameraGaitSettings VelocityDirection;

	UPROPERTY(EditAnywhere)
	FCameraGaitSettings LookingDirection;

	UPROPERTY(EditAnywhere)
	FCameraGaitSettings Aiming;
};

USTRUCT(BlueprintType)
struct FMantleAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere)
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere)
	FVector StartingOffset;

	UPROPERTY(EditAnywhere)
	float LowHeight = 0.0f;

	UPROPERTY(EditAnywhere)
	float LowPlayRate = 0.0f;

	UPROPERTY(EditAnywhere)
	float LowStartPosition = 0.0f;

	UPROPERTY(EditAnywhere)
	float HighHeight = 0.0f;

	UPROPERTY(EditAnywhere)
	float HighPlayRate = 0.0f;

	UPROPERTY(EditAnywhere)
	float HighStartPosition = 0.0f;
};

USTRUCT(BlueprintType)
struct FMantleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere)
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere)
	float StartingPosition = 0.0f;

	UPROPERTY(EditAnywhere)
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere)
	FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FMantleTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere)
	float MinLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere)
	float ReachDistance = 0.0f;

	UPROPERTY(EditAnywhere)
	float ForwardTraceRadius = 0.0f;

	UPROPERTY(EditAnywhere)
	float DownwardTraceRadius = 0.0f;
};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	float RunSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	float SprintSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	UCurveVector* MovementCurve = nullptr;

	UPROPERTY(EditAnywhere)
	UCurveFloat* RotationRateCurve = nullptr;

	float GetSpeedForGait(EGait Gait)
	{
		switch (Gait)
		{
		case EGait::Running:
			return RunSpeed;
		case EGait::Sprinting:
			return SprintSpeed;
		case EGait::Walking:
			return WalkSpeed;
		default:
			return RunSpeed;
		}
	}
};

USTRUCT(BlueprintType)
struct FMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FMovementSettings Standing;

	UPROPERTY(EditAnywhere)
	FMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FMovementStateSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FMovementStanceSettings VelocityDirection;

	UPROPERTY(EditAnywhere)
	FMovementStanceSettings LookingDirection;

	UPROPERTY(EditAnywhere)
	FMovementStanceSettings Aiming;
};

USTRUCT(BlueprintType)
struct FRotateInPlaceAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere)
	FName SlotName;

	UPROPERTY(EditAnywhere)
	float SlowTurnRate = 90.0f;

	UPROPERTY(EditAnywhere)
	float FastTurnRate = 90.0f;

	UPROPERTY(EditAnywhere)
	float SlowPlayRate = 1.0f;

	UPROPERTY(EditAnywhere)
	float FastPlayRate = 1.0f;
};
