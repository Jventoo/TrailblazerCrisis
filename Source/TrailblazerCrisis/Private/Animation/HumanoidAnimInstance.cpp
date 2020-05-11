// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/HumanoidAnimInstance.h"
#include "TCStatics.h"

#include "Actors/Characters/TCCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"

UHumanoidAnimInstance::UHumanoidAnimInstance()
{
	IsInAir = false;
	bReceivedInitDir = false;
	IsArmed = false;
	IsSprinting = false;
	IsAiming = false;

	bUseRootMotionValues = true;

	Speed = Direction = CurrDeltaTime = 0.f;

	bAllowTransition = true;

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

	CurrDeltaTime = DeltaSeconds;

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
	GetWorld()->GetTimerManager().ClearTimer(UpdateJumpHandle);
	GetWorld()->GetTimerManager().ClearTimer(AllowTransitionHandle);
}

void UHumanoidAnimInstance::SetReceivedDirTrue()
{
	bReceivedInitDir = true;
}

void UHumanoidAnimInstance::PlayTransition(FDynamicMontageParams Parameters)
{
	PlaySlotAnimationAsDynamicMontage(Parameters.Animation, TEXT("Grounded Slot"),
		Parameters.BlendInTime, Parameters.BlendOutTime, Parameters.PlayRate,
		1, 0.0f, Parameters.StartTime);
}

void UHumanoidAnimInstance::PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Parameters)
{
	if (bAllowTransition)
	{
		PlaySlotAnimationAsDynamicMontage(Parameters.Animation, TEXT("Grounded Slot"),
			Parameters.BlendInTime, Parameters.BlendOutTime, Parameters.PlayRate,
			1, 0.0f, Parameters.StartTime);

		bAllowTransition = false;

		// Allow retrigger after delay
		GetWorld()->GetTimerManager().SetTimer(AllowTransitionHandle, this,
			&UHumanoidAnimInstance::OpenTransition, ReTriggerDelay, false);
	}
}

void UHumanoidAnimInstance::OpenTransition()
{
	bAllowTransition = true;
}

void UHumanoidAnimInstance::Jumped_Implementation()
{
	bJumped = true;

	JumpPlayRate = UKismetMathLibrary::MapRangeClamped(Speed, 0, 600, 1.2, 1.5);

	GetWorld()->GetTimerManager().SetTimer(UpdateJumpHandle, this,
		&UHumanoidAnimInstance::SetJumpFalse, 0.1f, false);
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


void UHumanoidAnimInstance::UpdateCharacterInfo()
{
	if (Owner
		&& Owner->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		auto Vals = ICharacterInterface::Execute_GetEssentialValues(Owner);

		Velocity = Vals.Velocity;
		Acceleration = Vals.Acceleration;
		MovementInput = Vals.MovementInput;
		bIsMoving = Vals.IsMoving;
		HasMovementInput = Vals.HasMovementInput;
		Speed = Vals.Speed;
		MovementInputAmount = Vals.MovementInputAmt;
		AimingRotation = Vals.AimingRotation;
		AimYawRate = Vals.AimYawRate;

		auto States = ICharacterInterface::Execute_GetCurrentStates(Owner);

		MovementState = States.MovementState;
		PrevMovementState = States.PrevMovementState;
		MovementAction = States.MovementAction;
		RotationMode = States.VelocityDir;
		Gait = States.ActualGait;
		Stance = States.ActualStance;
		ViewMode = States.ViewMode;
		OverlayState = States.OverlayState;
	}
}

void UHumanoidAnimInstance::UpdateAimingValues()
{
	SmoothedAimingRotation = UKismetMathLibrary::RInterpTo(SmoothedAimingRotation,
		AimingRotation, CurrDeltaTime, SmoothedAimingRotationInterpSpeed);


	auto DeltaAimingAngle = UKismetMathLibrary::NormalizedDeltaRotator(
		AimingRotation, Owner->GetActorRotation());
	AimingAngle = FVector2D(DeltaAimingAngle.Yaw, DeltaAimingAngle.Pitch);

	auto DeltaSmoothedAimingAngle = UKismetMathLibrary::NormalizedDeltaRotator(
		SmoothedAimingRotation, Owner->GetActorRotation());
	SmoothedAimingAngle = FVector2D(DeltaAimingAngle.Yaw, DeltaAimingAngle.Pitch);


	if (RotationMode != ERotationMode::VelocityDirection)
	{
		AimSweepTime = UKismetMathLibrary::MapRangeClamped(AimingAngle.Y, -90, 90, 1, 0);

		SpineRotation = FRotator(0, AimingAngle.X / 4.0, 0);
	}


	if (RotationMode == ERotationMode::VelocityDirection && HasMovementInput)
	{
		float DeltaYawRotYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			UKismetMathLibrary::Conv_VectorToRotator(MovementInput), Owner->GetActorRotation()).Yaw;

		InputYawOffsetTime = UKismetMathLibrary::FInterpTo(InputYawOffsetTime,
			UKismetMathLibrary::MapRangeClamped(DeltaYawRotYaw, -100, 100, 0, 1),
			CurrDeltaTime, InputYawOffsetInterpSpeed);
	}

	auto SmoothedXABS = UKismetMathLibrary::Abs(SmoothedAimingAngle.X);

	LeftYawTime = UKismetMathLibrary::MapRangeClamped(SmoothedXABS, 0, 100, 0.5, 0);
	RightYawTime = UKismetMathLibrary::MapRangeClamped(SmoothedXABS, 0, 180, 0.5, 1);
	ForwardYawTime = UKismetMathLibrary::MapRangeClamped(SmoothedAimingAngle.X, -180, 180, 0, 1);
}

void UHumanoidAnimInstance::UpdateMovementValues()
{
	VelocityBlend = InterpVelocityBlend(VelocityBlend, CalculateVelocityBlend(), 
		VelocityBlendInterpSpeed, CurrDeltaTime);

	DiagonalScaleAmount = CalculateDiagonalScaleAmount();
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();

	LeanAmount = InterpLeanAmount(LeanAmount, FLeanAmount(RelativeAccelerationAmount.Y, 
		RelativeAccelerationAmount.X), GroundedLeanInterpSpeed, CurrDeltaTime);

	WalkRunBlend = CalculateWalkRunBlend();
	StrideBlend = CalculateStrideBlend();
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UHumanoidAnimInstance::UpdateRotationValues()
{
	MovementDirection = CalculateMovementDirection();

	auto DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(
		UKismetMathLibrary::Conv_VectorToRotator(Velocity), Owner->GetControlRotation()
	);

	auto TempFB = YawOffset_FB->GetVectorValue(DeltaRot.Yaw);
	auto TempLR = YawOffset_LR->GetVectorValue(DeltaRot.Yaw);

	FYaw = TempFB.X;
	BYaw = TempFB.Y;
	LYaw = TempLR.X;
	RYaw = TempLR.Y;
}

void UHumanoidAnimInstance::UpdateInAirValues()
{
	FallSpeed = Velocity.Z;
	LandPrediction = CalculateLandPrediction();
	LeanAmount = InterpLeanAmount(LeanAmount, CalculateInAirLeanAmount(), 
		InAirLeanInterpSpeed, CurrDeltaTime);
}

void UHumanoidAnimInstance::UpdateRagdollValues()
{
	FlailRate = UKismetMathLibrary::MapRangeClamped(
		GetOwningComponent()->GetPhysicsLinearVelocity().Size(), 0, 1000, 0, 1
	);
}


bool UHumanoidAnimInstance::ShouldMoveCheck() const
{
	return (Speed > 150.0f) || (bIsMoving && HasMovementInput);
}

bool UHumanoidAnimInstance::CanTurnInPlace() const
{
	return (RotationMode == ERotationMode::LookingDirection) 
		&& (ViewMode == EViewMode::ThirdPerson) 
		&& (GetCurveValue(TEXT("Enable_Transition")) > 0.99);
}

bool UHumanoidAnimInstance::CanRotateInPlace() const
{
	return ViewMode == EViewMode::FirstPerson ||
		RotationMode == ERotationMode::Aiming;
}

bool UHumanoidAnimInstance::CanDynamicTransition() const
{
	return GetCurveValue(TEXT("Enable_Transition")) == 1.0f;
}

bool UHumanoidAnimInstance::CanOverlayTransition() const
{
	return (Stance == EStance::Standing && !bShouldMove);
}

void UHumanoidAnimInstance::TurnInPlace(const FRotator& TargetRot, float PlayRateScale, float StartTime, bool OverrideCurrent)
{
	float TurnAngle = UKismetMathLibrary::NormalizedDeltaRotator(
		TargetRot, Owner->GetActorRotation()).Yaw;

	FTurnInPlace TargetTurnAsset;

	if (FMath::Abs(TurnAngle) < Turn180Threshold)
	{
		if (TurnAngle < 0)
		{
			switch (Stance)
			{
			case EStance::Standing:
				TargetTurnAsset = N_TurnIP_L90;
				break;

			case EStance::Crouching:
				TargetTurnAsset = CLF_TurnIP_L90;
				break;

			default:
				break;
			}
		}
		else
		{
			switch (Stance)
			{
			case EStance::Standing:
				TargetTurnAsset = N_TurnIP_R90;
				break;

			case EStance::Crouching:
				TargetTurnAsset = CLF_TurnIP_R90;
				break;

			default:
				break;
			}
		}
	}
	else
	{
		if (TurnAngle < 0)
		{
			switch (Stance)
			{
			case EStance::Standing:
				TargetTurnAsset = N_TurnIP_L180;
				break;

			case EStance::Crouching:
				TargetTurnAsset = CLF_TurnIP_L180;
				break;

			default:
				break;
			}
		}
		else
		{
			switch (Stance)
			{
			case EStance::Standing:
				TargetTurnAsset = N_TurnIP_R180;
				break;

			case EStance::Crouching:
				TargetTurnAsset = CLF_TurnIP_R180;
				break;

			default:
				break;
			}
		}
	}

	if (TargetTurnAsset.Animation)
	{
		if (OverrideCurrent
			|| !IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
		{
			PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName,
				0.2, 0.2, TargetTurnAsset.PlayRate * PlayRateScale, 1, 0, StartTime);


			RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;

			if (TargetTurnAsset.ScaleTurnAngle)
				RotationScale *= (TurnAngle / TargetTurnAsset.AnimatedAngle);
		}
	}
}

void UHumanoidAnimInstance::TurnInPlaceCheck()
{
	float AngleAbs = FMath::Abs(AimingAngle.X);

	if (AngleAbs > TurnCheckMinAngle && AimYawRate < AimYawRateLimit)
	{
		ElapsedDelayTime += CurrDeltaTime;

		bool ExceedsDelay = ElapsedDelayTime > (UKismetMathLibrary::MapRangeClamped(
			AngleAbs, TurnCheckMinAngle, 180, MinAngleDelay, MaxAngleDelay));

		if (ExceedsDelay)
			TurnInPlace(FRotator(0, AimingRotation.Yaw, 0), 1.0, 0.0, false);
	}
	else
		ElapsedDelayTime = 0.0f;
}

void UHumanoidAnimInstance::RotateInPlaceCheck()
{
	Rotate_L = AimingAngle.X < RotateMinThreshold;
	Rotate_R = AimingAngle.X > RotateMaxThreshold;

	if (Rotate_L || Rotate_R)
	{
		RotateRate = UKismetMathLibrary::MapRangeClamped(
			AimYawRate, AimYawRateMinRange, AimYawRateMaxRange, MinPlayRate, MaxPlayRate);
	}
}


FVelocityBlend UHumanoidAnimInstance::CalculateVelocityBlend()
{
	auto VelocityCopy = Velocity;
	VelocityCopy.Normalize(0.1);
	FVector LocVelacityBlendDir = Owner->GetActorRotation().UnrotateVector(VelocityCopy);

	float Sum = FMath::Abs(LocVelacityBlendDir.X)
		+ FMath::Abs(LocVelacityBlendDir.Y)
		+ FMath::Abs(LocVelacityBlendDir.Z);

	FVector RelativeDir = LocVelacityBlendDir / Sum;

	float F = UKismetMathLibrary::Clamp(RelativeDir.X, 0, 1);
	float B = FMath::Abs(UKismetMathLibrary::Clamp(RelativeDir.X, -1, 0));
	float L = FMath::Abs(UKismetMathLibrary::Clamp(RelativeDir.Y, -1, 0));
	float R = UKismetMathLibrary::Clamp(RelativeDir.Y, 0, 1);

	return FVelocityBlend(F, B, L, R);
}

float UHumanoidAnimInstance::CalculateDiagonalScaleAmount()
{
	return DiagonalScaleAmountCurve->GetFloatValue(FMath::Abs(VelocityBlend.F + VelocityBlend.B));
}

FVector UHumanoidAnimInstance::CalculateRelativeAccelerationAmount()
{
	float Accel = 0.0;

	auto Character = Cast<ACharacter>(Owner);
	if (Character)
	{
		if (UKismetMathLibrary::Dot_VectorVector(Acceleration, Velocity) > 0)
			Accel = Character->GetCharacterMovement()->GetMaxAcceleration();
		else
			Accel = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();

		return (Owner->GetActorRotation().UnrotateVector(
			UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, Accel) / Accel));
	}
	return FVector::ZeroVector;
}

float UHumanoidAnimInstance::CalculateWalkRunBlend()
{
	switch (Gait)
	{
	case EGait::Walking:
		return 0.0f;

	default:
		return 1.0f;
	}
}

float UHumanoidAnimInstance::CalculateStrideBlend()
{
	auto A = UKismetMathLibrary::Lerp(StrideBlend_N_Walk->GetFloatValue(Speed),
		StrideBlend_N_Run->GetFloatValue(Speed),
		GetAnimCurve_Clamped(TEXT("Weight_Gait"), -1, 0, 1));

	return UKismetMathLibrary::Lerp(A, StrideBlend_C_Walk->GetFloatValue(Speed),
		GetCurveValue(TEXT("BasePose_CLF")));
}

float UHumanoidAnimInstance::CalculateStandingPlayRate()
{
	auto A = UKismetMathLibrary::Lerp(Speed / AnimatedWalkSpeed, Speed / AnimatedRunSpeed,
		GetAnimCurve_Clamped(TEXT("Weight_Gait"), -1, 0, 1));

	auto Lerped = UKismetMathLibrary::Lerp(A, Speed / AnimatedSprintSpeed,
		GetAnimCurve_Clamped(TEXT("Weight_Gait"), -2, 0, 1));

	auto SceneComp = Cast<USceneComponent>(GetOwningComponent());

	if (SceneComp)
		return UKismetMathLibrary::Clamp((Lerped / StrideBlend) / SceneComp->GetComponentScale().Z, 0, 3);
	else
		return 0.0f;
}

float UHumanoidAnimInstance::CalculateCrouchingPlayRate()
{
	auto SceneComp = Cast<USceneComponent>(GetOwningComponent());

	if (SceneComp)
	{
		float Val = Speed / AnimatedCrouchSpeed / StrideBlend / SceneComp->GetComponentScale().Z;
		return UKismetMathLibrary::Clamp(Val, 0, 2);
	}
	else
		return 0.0f;
}


float UHumanoidAnimInstance::CalculateLandPrediction()
{
	auto Character = Cast<ACharacter>(Owner);

	if (Character)
	{
		auto Capsule = Character->GetCapsuleComponent();

		if (Capsule && FallSpeed < -200.0f)
		{
			auto SceneComp = Cast<USceneComponent>(Capsule);
			auto CapsuleLoc = SceneComp->GetComponentLocation();

			auto UnsafeVect = UKismetMathLibrary::Vector_NormalUnsafe(
				FVector(Velocity.X, Velocity.Y, UKismetMathLibrary::Clamp(Velocity.Z, -4000.0, -200.0))
			);

			auto VectMultiplier = UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0, -4000.0, 50.0, 2000.0);

			TArray<AActor*> Ignore;
			FHitResult OutHit;

			UKismetSystemLibrary::CapsuleTraceSingleByProfile(GetWorld(), CapsuleLoc, CapsuleLoc + (UnsafeVect * VectMultiplier),
				Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight(), TEXT("TC_Character"), false, 
				Ignore, EDrawDebugTrace::None, OutHit, true);

			if (Character->GetCharacterMovement()->IsWalkable(OutHit) && OutHit.bBlockingHit)
			{
				return UKismetMathLibrary::Lerp(
					LandPredictionCurve->GetFloatValue(OutHit.Time), 0.0, GetCurveValue(TEXT("Mask_LandPrediction"))
				);
			}
		}
	}
	return 0.0f;
}

FLeanAmount UHumanoidAnimInstance::CalculateInAirLeanAmount()
{
	FVector Unrotated = Owner->GetActorRotation().UnrotateVector(Velocity) / 350.0f;
	FVector2D RelativeVelocity(Unrotated.Y, Unrotated.X);

	RelativeVelocity *= LeanInAirCurve->GetFloatValue(FallSpeed);

	return FLeanAmount(RelativeVelocity.X, RelativeVelocity.Y);
}


void UHumanoidAnimInstance::SetFootOffsets(const FName& Enable_FootIK_Curve, const FName& IKFootBone, const FName& RootBone, 
	FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset)
{
	auto SceneComp = Cast<USceneComponent>(GetOwningComponent());

	FVector ImpactPoint, ImpactNormal;
	FRotator TargetRotationOffset;

	// Clear offsets if weight = 0
	if (SceneComp && GetCurveValue(Enable_FootIK_Curve) > 0)
	{
		// Trace downwards
		auto FootSocket = SceneComp->GetSocketLocation(IKFootBone);

		FVector IKFootFloorLoc(FootSocket.X, FootSocket.Y, SceneComp->GetSocketLocation(RootBone).Z);

		auto StartVect = FVector(IKFootFloorLoc.X, IKFootFloorLoc.Y, IKFootFloorLoc.Z + IK_TraceDistanceAboveFoot);
		auto EndVect = FVector(IKFootFloorLoc.X, IKFootFloorLoc.Y, IKFootFloorLoc.Z - IK_TraceDistanceBelowFoot);
		
		TArray<AActor*> IgnoreActors;
		FHitResult HitInfo;

		UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartVect, EndVect, ETraceTypeQuery::TraceTypeQuery1, false,
			IgnoreActors, EDrawDebugTrace::None, HitInfo, true);

		auto Character = Cast<ACharacter>(Owner);

		// Find delta for angled surfaces and rotation offset
		if (Character && Character->GetCharacterMovement()->IsWalkable(HitInfo))
		{
			ImpactPoint = HitInfo.ImpactPoint;
			ImpactNormal = HitInfo.ImpactNormal;

			CurrentLocationTarget = (ImpactNormal * FootHeight) + ImpactPoint 
				- (IKFootFloorLoc + (FVector(0, 0, 1) * FootHeight));

			TargetRotationOffset = FRotator(UKismetMathLibrary::Atan2(ImpactNormal.Y, ImpactNormal.Z), 0, -1.0 * UKismetMathLibrary::Atan2(ImpactNormal.X, ImpactNormal.Z));
		}

		// Interp the current loc to the new target. Speed depends on above or below
		if (CurrentLocationOffset.Z > CurrentLocationTarget.Z)
		{
			CurrentLocationOffset = UKismetMathLibrary::VInterpTo(
				CurrentLocationOffset, CurrentLocationTarget, CurrDeltaTime, 30.0f);
		}
		else
		{
			CurrentLocationOffset = UKismetMathLibrary::VInterpTo(
				CurrentLocationOffset, CurrentLocationTarget, CurrDeltaTime, 15.0f);
		}

		// Interp the rotation offset
		CurrentRotationOffset = UKismetMathLibrary::RInterpTo(
			CurrentRotationOffset, TargetRotationOffset, CurrDeltaTime, 30.0f);

	}
	else
	{
		// Clear offsets when curve = 0
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
	}
}

void UHumanoidAnimInstance::SetPelvisIKOffset(const FVector& FootOffset_L_Target, const FVector& FootOffset_R_Target)
{
	// Find average foot IK weight
	PelvisAlpha = (GetCurveValue(TEXT("Enable_FootIK_L")) + GetCurveValue(TEXT("Enable_FootIK_R"))) / 2.0f;

	if (PelvisAlpha > 0)
	{
		FVector PelvisTarget;

		// Set new target to lowest offset
		if (FootOffset_L_Target.Z < FootOffset_R_Target.Z)
			PelvisTarget = FootOffset_L_Target;
		else
			PelvisTarget = FootOffset_R_Target;

		// Interp to new offset
		if (PelvisTarget.Z > PelvisOffset.Z)
		{
			PelvisOffset = UKismetMathLibrary::VInterpTo(PelvisOffset, PelvisTarget, CurrDeltaTime, 10.0f);
		}
		else
		{
			PelvisOffset = UKismetMathLibrary::VInterpTo(PelvisOffset, PelvisTarget, CurrDeltaTime, 15.0f);
		}
	}
	else
		PelvisOffset = FVector::ZeroVector;
}

void UHumanoidAnimInstance::SetFootLocking(const FName& Enable_FootIK_Curve, const FName& FootLockCurve, const FName& IKFootBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation)
{
	// Only update if curve has weight
	if (GetCurveValue(Enable_FootIK_Curve) > 0)
	{
		float FootLockCurveVal = GetCurveValue(FootLockCurve);

		if (FootLockCurveVal >= 0.99 || FootLockCurveVal < CurrentFootLockAlpha)
			CurrentFootLockAlpha = FootLockCurveVal;

		if (CurrentFootLockAlpha >= 0.99)
		{
			auto SceneComp = Cast<USceneComponent>(GetOwningComponent());

			if (SceneComp)
			{
				auto SocketTrans = SceneComp->GetSocketTransform(IKFootBone);

				CurrentFootLockLocation = SocketTrans.GetLocation();
				CurrentFootLockRotation = SocketTrans.Rotator();
			}
		}

		if (CurrentFootLockAlpha > 0)
			SetFootLockOffsets(CurrentFootLockLocation, CurrentFootLockRotation);
	}
}

void UHumanoidAnimInstance::SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation)
{
	FRotator RotDifference;
	FVector LocDifference;

	// Find how much we need to rotate
	auto Character = Cast<ACharacter>(Owner);
	if (Character && Character->GetCharacterMovement()->IsMovingOnGround())
		RotDifference = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetActorRotation(), Character->GetCharacterMovement()->GetLastUpdateRotation());

	// Get distance traveled between frames
	auto SceneComp = Cast<USceneComponent>(GetOwningComponent());
	if (SceneComp)
		LocDifference = SceneComp->GetComponentRotation().UnrotateVector(Velocity * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	LocalLocation = UKismetMathLibrary::RotateAngleAxis(LocalLocation - LocDifference, RotDifference.Yaw, FVector(0, 0, -1));
	LocalRotation = UKismetMathLibrary::NormalizedDeltaRotator(LocalRotation, RotDifference);
}


EMovementDirection UHumanoidAnimInstance::CalculateMovementDirection()
{
	if (Gait == EGait::Sprinting)
		return EMovementDirection::Forward;
	else
	{
		if (RotationMode == ERotationMode::VelocityDirection)
			return EMovementDirection::Forward;
		else
		{
			return CalculateQuadrant(MovementDirection, 70, -70, 110, -110, 5,
				UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::Conv_VectorToRotator(Velocity), AimingRotation).Yaw
				);
		}
	}
}

EMovementDirection UHumanoidAnimInstance::CalculateQuadrant(EMovementDirection CurrDir, float FRThreshold, float FLThreshold, float BRThreshold, float BLThreshold, float Buffer, float Angle)
{
	if (AngleInRange(Angle, FLThreshold, FRThreshold, Buffer, (CurrDir != EMovementDirection::Forward || CurrDir != EMovementDirection::Backward)))
		return EMovementDirection::Forward;
	else if (AngleInRange(Angle, FRThreshold, BRThreshold, Buffer, (CurrDir != EMovementDirection::Right || CurrDir != EMovementDirection::Left)))
		return EMovementDirection::Right;
	else if (AngleInRange(Angle, BLThreshold, FLThreshold, Buffer, (CurrDir != EMovementDirection::Right || CurrDir != EMovementDirection::Left)))
		return EMovementDirection::Left;
	else
		return EMovementDirection::Backward;
}

bool UHumanoidAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer)
{
	if (IncreaseBuffer)
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle - Buffer, MaxAngle + Buffer);
	}
	else
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle + Buffer, MaxAngle - Buffer);
	}
}


FVelocityBlend UHumanoidAnimInstance::InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed, float DeltaTime)
{
	float F = UKismetMathLibrary::FInterpTo(Current.F, Target.F, DeltaTime, InterpSpeed);
	float B = UKismetMathLibrary::FInterpTo(Current.B, Target.B, DeltaTime, InterpSpeed);
	float L = UKismetMathLibrary::FInterpTo(Current.L, Target.L, DeltaTime, InterpSpeed);
	float R = UKismetMathLibrary::FInterpTo(Current.R, Target.R, DeltaTime, InterpSpeed);

	return FVelocityBlend(F, B, L, R);
}

FLeanAmount UHumanoidAnimInstance::InterpLeanAmount(FLeanAmount Current, FLeanAmount Target, float InterpSpeed, float DeltaTime)
{
	float LR = UKismetMathLibrary::FInterpTo(Current.LR, Target.LR, DeltaTime, InterpSpeed);
	float FB = UKismetMathLibrary::FInterpTo(Current.FB, Target.FB, DeltaTime, InterpSpeed);
	
	return FLeanAmount(LR, FB);
}

void UHumanoidAnimInstance::ResetIKOffsets()
{
	FootOffset_L_Location = UKismetMathLibrary::VInterpTo(FootOffset_L_Location,
		FVector::ZeroVector, CurrDeltaTime, 15);

	FootOffset_R_Location = UKismetMathLibrary::VInterpTo(FootOffset_R_Location,
		FVector::ZeroVector, CurrDeltaTime, 15);

	FootOffset_L_Rotation = UKismetMathLibrary::RInterpTo(FootOffset_L_Rotation,
		FRotator::ZeroRotator, CurrDeltaTime, 15);

	FootOffset_R_Rotation = UKismetMathLibrary::RInterpTo(FootOffset_R_Rotation,
		FRotator::ZeroRotator, CurrDeltaTime, 15);
}

float UHumanoidAnimInstance::GetAnimCurve_Clamped(FName Name, float Bias, float Min, float Max)
{
	return UKismetMathLibrary::Clamp(GetCurveValue(Name) + Bias, Min, Max);
}