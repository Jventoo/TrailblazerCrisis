// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/HumanoidAnimInstance.h"
#include "TCStatics.h"

#include "Actors/Characters/TCCharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UHumanoidAnimInstance::UHumanoidAnimInstance()
{
	IsInAir = false;
	bReceivedInitDir = false;
	IsArmed = false;
	IsSprinting = false;
	IsAiming = false;

	bUseRootMotionValues = true;

	Speed = Direction = 0.f;

	// Character State
	bIsMoving = HasMovementInput = false;
	Speed = MovementInputAmount = AimYawRate = ZoomAmount = 0.0f;

	// Grounded
	bShouldMove = Rotate_L = Rotate_R = Pivot = false;
	RotationScale = DiagonalScaleAmount = WalkRunBlend = StrideBlend = 0.0f;
	FYaw = BYaw = LYaw = RYaw = 0.0f;
	RotateRate = StandingPlayRate = CrouchingPlayRate = 1.0f;

	// In air
	bJumped = false;
	JumpPlayRate = 1.2f;
	FallSpeed = 0.0f;
	LandPrediction = 1.0f;

	// Aiming
	AimSweepTime = 0.5f;
	InputYawOffsetTime = ForwardYawTime = LeftYawTime = RightYawTime = 0.0f;

	// Ragdoll
	FlailRate = 0.0f;

	// Layer Blending
	OverlayOverrideState = 0;
	Enable_AimOffset = BasePose_N = Enable_HandIK_L = Enable_HandIK_R = 1.0f;
	BasePose_CLF = Hand_L = Hand_R = 0.0f;
	Arm_L = Arm_L_Add = Arm_L_LS = Arm_L_MS = 0.0f;
	Arm_R = Arm_R_Add = Arm_R_LS = Arm_R_MS = 0.0f;
	Legs = Legs_Add = Pelvis = Pelvis_Add = Spine = Spine_Add = Head = Head_Add = 0.0f;

	// Foot IK
	FootLock_L_Alpha = FootLock_R_Alpha = PelvisAlpha = 0.0f;

	// Turn In Place
	TurnCheckMinAngle = 45.0f;
	Turn180Threshold = 130.0f;
	AimYawRateLimit = 50.0f;
	ElapsedDelayTime = MaxAngleDelay = 0.0f;
	MinAngleDelay = 0.75f;

	// Rot In Place
	RotateMinThreshold = -50.0f;
	RotateMaxThreshold = 50.0f;
	AimYawRateMinRange = 90.0f;
	AimYawRateMaxRange = 270.0f;
	MinPlayRate = 1.15f;
	MaxPlayRate = 3.0f;

	// Config
	AnimatedWalkSpeed = 150.0f;
	AnimatedRunSpeed = 350.0f;
	AnimatedSprintSpeed = 600.0f;
	AnimatedCrouchSpeed = 150.0f;
	VelocityBlendInterpSpeed = 12.0f;
	GroundedLeanInterpSpeed = 4.0f;
	InAirLeanInterpSpeed = 4.0f;
	SmoothedAimingRotationInterpSpeed = 10.0f;
	InputYawOffsetInterpSpeed = 8.0f;
	TriggerPivotSpeedLimit = 200.0f;
	FootHeight = 13.5;
	IK_TraceDistanceAboveFoot = 50.0;
	IK_TraceDistanceBelowFoot = 45.0;
}

void UHumanoidAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = TryGetPawnOwner();
}

void UHumanoidAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
		return;

	if (Owner->IsA(ATCCharacterBase::StaticClass()))
	{
		UpdateCharacterInfo();
		UpdateAimingValues();
		UpdateLayerValues();
		UpdateFootIK();

		switch (MovementState)
		{
		case EMovementState::Grounded:

			break;

		case EMovementState::InAir:
			UpdateInAirValues();
			break;

		case EMovementState::Ragdoll:
			UpdateRagdollValues();
			break;

		default:
			break;
		}

		ATCCharacterBase* Character = Cast<ATCCharacterBase>(Owner);
		if (Character)
		{
			if (bUseRootMotionValues)
			{
				IsInAir = Character->GetMovementComponent()->IsFalling();

				// Set speed to the sum of our abs inputs (max 1.0f) or our sprint speed
				auto Sum = FMath::Min(Character->GetForwardAxisValue(true)
					+ Character->GetRightAxisVal(true), UTCStatics::MAX_MOVE_SPEED);
				Speed = IsSprinting ? (Sum * UTCStatics::SPRINT_MODIFIER) : Sum;

				if (Speed > 0.01)
				{
					if (!bReceivedInitDir)
					{
						Direction = Character->GetDirection();

						if (!GetWorld()->GetTimerManager().IsTimerActive(UpdateReceivedHandle))
						{
							GetWorld()->GetTimerManager().SetTimer(UpdateReceivedHandle, this,
								&UHumanoidAnimInstance::SetReceivedDirTrue, 0.1f, false);
						}
					}
				}
				else
				{
					bReceivedInitDir = false;
				}
			}
			else
			{
				Speed = Owner->GetVelocity().Size();
				Direction = CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
			}
		}
	}
}

void UHumanoidAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();

	GetWorld()->GetTimerManager().ClearTimer(UpdateReceivedHandle);
}


void UHumanoidAnimInstance::Jumped_Implementation()
{
	bJumped = true;

	JumpPlayRate = UKismetMathLibrary::MapRangeClamped(Speed, 0, 600, 1.2, 1.5);

	GetWorld()->GetTimerManager().SetTimer(UpdateJumpHandle, this,
		&UHumanoidAnimInstance::SetJumpFalse, 0.1f, false);
}

void UHumanoidAnimInstance::SetReceivedDirTrue()
{
	bReceivedInitDir = true;
}

void UHumanoidAnimInstance::SetJumpFalse()
{
	bJumped = false;
}

void UHumanoidAnimInstance::SetGroundedEntryState_Implementation(EGroundedEntryState State)
{
	GroundedEntryState = State;
}

void UHumanoidAnimInstance::SetOverlayOverrideState_Implementation(int32 NewOverlayOverrideState)
{
	OverlayOverrideState = NewOverlayOverrideState;
}