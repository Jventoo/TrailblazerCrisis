// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/AnimationInterface.h"
#include "HumanoidAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EHipsDirection : uint8
{
	F,
	B,
	RF,
	RB,
	LF,
	LB
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_USTRUCT_BODY()

	FDynamicMontageParams()
	{
		Animation = nullptr;
		BlendInTime = BlendOutTime = PlayRate = StartTime = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimSequenceBase* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		float BlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		float BlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		float StartTime;
};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_USTRUCT_BODY()

	FVelocityBlend()
	{
		F = B = L = R = 0.0f;
	}
	
	FVelocityBlend(float newF, float newB, float newL, float newR)
		: F(newF), B(newB), L(newL), R(newR)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity")
		float F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity")
		float B;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity")
		float L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity")
		float R;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_USTRUCT_BODY()

	FLeanAmount()
	{
		LR = FB = 0.0f;
	}

	FLeanAmount(float newLR, float newFB)
		: LR(newLR), FB(newFB)
	{

	}

	// Left-right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean")
		float LR;

	// Front-back
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean")
		float FB;
};

USTRUCT(BlueprintType)
struct FTurnInPlace
{
	GENERATED_USTRUCT_BODY()

	FTurnInPlace()
	{
		AnimatedAngle = 0.0f;
		PlayRate = 1.0f;
		ScaleTurnAngle = true;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		UAnimSequenceBase* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float AnimatedAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		bool ScaleTurnAngle;
};

USTRUCT(BlueprintType)
struct FRotateInPlace
{
	GENERATED_USTRUCT_BODY()

	FRotateInPlace()
	{
		SlowTurnRate = FastTurnRate = 90.0f;
		SlowPlayRate = FastPlayRate = 1.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		UAnimSequenceBase* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float SlowTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float FastTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float SlowPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float FastPlayRate;
};


/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UHumanoidAnimInstance : public UAnimInstance, public IAnimationInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bReceivedInitDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bUseRootMotionValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		TArray<class UPhysicalMaterial*> FloorTypes;

private:
	class APawn* Owner;

	FTimerHandle UpdateReceivedHandle;

	FTimerHandle UpdateJumpHandle;

	FTimerHandle AllowTransitionHandle;

	bool bAllowTransition;

public:
	UHumanoidAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeUninitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "Montage")
		void PlayTransition(FDynamicMontageParams Parameters);

	UFUNCTION(BlueprintCallable, Category = "Montage")
		void PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Parameters);

	/************************************************************************/
	/* Interface																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void Jumped();
	virtual void Jumped_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetGroundedEntryState(EGroundedEntryState State);
	virtual void SetGroundedEntryState_Implementation(EGroundedEntryState State) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetOverlayOverrideState(int32 NewOverlayOverrideState);
	virtual void SetOverlayOverrideState_Implementation(int32 NewOverlayOverrideState) override;

	/************************************************************************/
	/* Updates																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateCharacterInfo();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateAimingValues();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Updates")
		void UpdateLayerValues();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateFootIK();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateMovementValues();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateRotationValues();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateInAirValues();

	UFUNCTION(BlueprintCallable, Category = "Updates")
		void UpdateRagdollValues();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character States")
		float CurrDeltaTime;

	/************************************************************************/
	/* Character Info														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		FRotator AimingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		FVector RelativeVelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		FVector Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		FVector MovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		bool HasMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		float MovementInputAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		float AimYawRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		float ZoomAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EMovementState MovementState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EMovementState PrevMovementState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EMovementAction MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		ERotationMode RotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EGait Gait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EStance Stance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character States")
		EViewMode ViewMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		EOverlayState OverlayState;

	/************************************************************************/
	/* Grounded																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grounded")
		bool ShouldMoveCheck() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grounded")
		bool CanTurnInPlace() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grounded")
		bool CanRotateInPlace() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grounded")
		bool CanDynamicTransition() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grounded")
		bool CanOverlayTransition() const;

	UFUNCTION(BlueprintCallable, Category = "Grounded")
		void TurnInPlace(const FRotator& TargetRot, float PlayRateScale,
			float StartTime, bool OverrideCurrent);

	UFUNCTION(BlueprintCallable, Category = "Grounded")
		void TurnInPlaceCheck();

	UFUNCTION(BlueprintCallable, Category = "Grounded")
		void RotateInPlaceCheck();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Grounded")
		void DynamicTransitionCheck();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		EGroundedEntryState GroundedEntryState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		EMovementDirection MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		EHipsDirection TrackedHipsDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		FVector RelativeAccelerationAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		bool Rotate_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		bool Rotate_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		bool Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float RotateRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float RotationScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float DiagonalScaleAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float WalkRunBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float StandingPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float CrouchingPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float StrideBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		FVelocityBlend VelocityBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		FLeanAmount LeanAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float FYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float BYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float LYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded")
		float RYaw;

	/************************************************************************/
	/* Movement																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		FVelocityBlend CalculateVelocityBlend();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float CalculateDiagonalScaleAmount();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		FVector CalculateRelativeAccelerationAmount();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float CalculateWalkRunBlend();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float CalculateStrideBlend();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float CalculateStandingPlayRate();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float CalculateCrouchingPlayRate();

	/************************************************************************/
	/* InAir																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "InAir")
		float CalculateLandPrediction();

	UFUNCTION(BlueprintCallable, Category = "InAir")
		FLeanAmount CalculateInAirLeanAmount();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InAir")
		bool bJumped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InAir")
		float JumpPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InAir")
		float FallSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InAir")
		float LandPrediction;

	/************************************************************************/
	/* Aiming Values														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		FRotator SmoothedAimingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		FRotator SpineRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		FVector2D AimingAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		FVector2D SmoothedAimingAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float AimSweepTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float InputYawOffsetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float ForwardYawTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float LeftYawTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float RightYawTime;

	/************************************************************************/
	/* Ragdoll																*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ragdoll")
		float FlailRate;

	/************************************************************************/
	/* Layer Blending														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		int32 OverlayOverrideState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Enable_AimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float BasePose_N;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float BasePose_CLF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_L_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_L_LS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_L_MS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_R_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_R_LS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Arm_R_MS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Hand_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Hand_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Legs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Legs_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Pelvis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Pelvis_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Spine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Spine_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Head_Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Enable_HandIK_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layer Blending")
		float Enable_HandIK_R;

	/************************************************************************/
	/* Foot IK																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetFootOffsets(
			const FName& Enable_FootIK_Curve, const FName& IKFootBone,
			const FName& RootBone, FVector& CurrentLocationTarget,
			FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetPelvisIKOffset(const FVector& FootOffset_L_Target,
			const FVector& FootOffset_R_Target);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetFootLocking(
			const FName& Enable_FootIK_Curve, const FName& FootLockCurve,
			const FName& IKFootBone, float& CurrentFootLockAlpha,
			FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		float FootLock_L_Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		float FootLock_R_Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FVector FootLock_L_Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FVector FootLock_R_Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FRotator FootLock_L_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FRotator FootLock_R_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FVector FootOffset_L_Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FVector FootOffset_R_Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FRotator FootOffset_L_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FRotator FootOffset_R_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		FVector PelvisOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot IK")
		float PelvisAlpha;

	/************************************************************************/
	/* TIP																	*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float TurnCheckMinAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float Turn180Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float AimYawRateLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float ElapsedDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float MinAngleDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		float MaxAngleDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace N_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace N_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace N_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace N_TurnIP_R180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace CLF_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace CLF_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace CLF_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn In Place")
		FTurnInPlace CLF_TurnIP_R180;

	/************************************************************************/
	/* RIP																	*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		EMovementDirection CalculateMovementDirection();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		EMovementDirection CalculateQuadrant(EMovementDirection CurrDir,
			float FRThreshold, float FLThreshold, float BRThreshold,
			float BLThreshold, float Buffer, float Angle);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		bool AngleInRange(float Angle, float MinAngle, float MaxAngle,
			float Buffer, bool IncreaseBuffer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float RotateMinThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float RotateMaxThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float AimYawRateMinRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float AimYawRateMaxRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float MinPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate In Place")
		float MaxPlayRate;

	/************************************************************************/
	/* Blend Curves															*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* DiagonalScaleAmountCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* StrideBlend_N_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* StrideBlend_N_Run;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* StrideBlend_C_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* LandPredictionCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveFloat* LeanInAirCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveVector* YawOffset_FB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
		class UCurveVector* YawOffset_LR;

	/************************************************************************/
	/* Config																*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float AnimatedWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float AnimatedRunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float AnimatedSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float AnimatedCrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float VelocityBlendInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float GroundedLeanInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float InAirLeanInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float SmoothedAimingRotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float InputYawOffsetInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float TriggerPivotSpeedLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float FootHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float IK_TraceDistanceAboveFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float IK_TraceDistanceBelowFoot;

	/************************************************************************/
	/* Utility																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		FVelocityBlend InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target,
			float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		FLeanAmount InterpLeanAmount(FLeanAmount Current, FLeanAmount Target,
			float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		float GetAnimCurve_Clamped(FName Name, float Bias, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = "Foot IK")
		void ResetIKOffsets();

private:

	UFUNCTION()
		void SetReceivedDirTrue();

	UFUNCTION()
		void SetJumpFalse();

	UFUNCTION()
		void OpenTransition();
};
