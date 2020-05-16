// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "TCStatics.generated.h"

/************************************************************************/
/* Animation															*/
/************************************************************************/

USTRUCT(BlueprintType)
struct FAnimCurveCreation
{
	GENERATED_USTRUCT_BODY()

		FAnimCurveCreation()
	{
		FrameNumber = 0;
		CurveValue = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		int32 FrameNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		float CurveValue;
};

USTRUCT(BlueprintType)
struct FAnimCurveCreationParams
{
	GENERATED_USTRUCT_BODY()

		FAnimCurveCreationParams()
	{
		CurveName = TEXT("None");
		KeyEachFrame = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		FName CurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		bool KeyEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimModifier")
		TArray<FAnimCurveCreation> Keys;
};


/************************************************************************/
/* Locomotion															*/
/************************************************************************/

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

USTRUCT(BlueprintType)
struct FTransformAndComp
{
	GENERATED_USTRUCT_BODY()

	FTransformAndComp() {}

	FTransformAndComp(const FTransform& NewTransform, UPrimitiveComponent* NewComp)
		: Transform(NewTransform)
	{
		Comp = NewComp;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformAndComp")
		FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformAndComp")
		class UPrimitiveComponent* Comp;
};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_USTRUCT_BODY()

	FMovementSettings() 
	{
		WalkSpeed = RunSpeed = SprintSpeed = 0.0f;
		MovementCurve = nullptr;
		RotationRateCurve = nullptr;
	}

	FMovementSettings(float NewWalk, float NewRun, float NewSprint, class UCurveVector* MoveCurve, class UCurveFloat* RotCurve)
		: WalkSpeed(NewWalk), RunSpeed(NewRun), SprintSpeed(NewSprint)
	{
		MovementCurve = MoveCurve;
		RotationRateCurve = RotCurve;
	}

	FMovementSettings(const FMovementSettings& OtherStruct)
	{
		WalkSpeed = OtherStruct.WalkSpeed;
		RunSpeed = OtherStruct.RunSpeed;
		SprintSpeed = OtherStruct.SprintSpeed;
		MovementCurve = OtherStruct.MovementCurve;
		RotationRateCurve = OtherStruct.RotationRateCurve;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		class UCurveVector* MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		class UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct FMovementSettings_Stance
{
	GENERATED_USTRUCT_BODY()

	FMovementSettings_Stance() {}

	FMovementSettings_Stance(FMovementSettings Stand, FMovementSettings Crouch)
		: Standing(Stand), Crouching(Crouch) {}

	FMovementSettings_Stance(const FMovementSettings_Stance& Other)
		: Standing(Other.Standing), Crouching(Other.Crouching) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		FMovementSettings Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		FMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FMovementSettings_State : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FMovementSettings_State() {}

	FMovementSettings_State(FMovementSettings_Stance VelDir, FMovementSettings_Stance LookDir, FMovementSettings_Stance Aim)
		: VelocityDirection(VelDir), LookingDirection(LookDir), Aiming(Aim)
	{}

	FMovementSettings_State(const FMovementSettings_State& Other)
		: VelocityDirection(Other.VelocityDirection), LookingDirection(Other.LookingDirection), Aiming(Other.Aiming)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		FMovementSettings_Stance VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		FMovementSettings_Stance LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSettings")
		FMovementSettings_Stance Aiming;
};

USTRUCT(BlueprintType)
struct FMantleAsset
{
	GENERATED_USTRUCT_BODY()

	FMantleAsset()
	{
		AnimMontage = nullptr;
		PositionCorrectCurve = nullptr;
		LowHeight = LowPlayRate = LowStartPosition = HighHeight = HighPlayRate = HighStartPosition = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		class UCurveVector* PositionCorrectCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		FVector StartingOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float LowHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float LowPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float LowStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float HighHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float HighPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float HighStartPosition;
};


USTRUCT(BlueprintType)
struct FMantleParams
{
	GENERATED_USTRUCT_BODY()

	FMantleParams()
	{
		AnimMontage = nullptr;
		PositionCorrectCurve = nullptr;
		StartingPosition = PlayRate = 0.0f;
	}

	FMantleParams(UAnimMontage* NewMontage, UCurveVector* NewCurve, float StartPos, float NewRate, const FVector& NewStart)
		: StartingPosition(StartPos), PlayRate(NewRate), StartingOffset(NewStart)
	{
		AnimMontage = NewMontage;
		PositionCorrectCurve = NewCurve;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		class UCurveVector* PositionCorrectCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float StartingPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FMantleTraceSettings
{
	GENERATED_USTRUCT_BODY()

	FMantleTraceSettings()
	{
		MaxLedgeHeight = MinLedgeHeight = ReachDistance = 
			ForwardTraceRadius = DownwardTraceRadius = 0.0f;
	}

	FMantleTraceSettings(float MaxHeight, float MinHeight, float ReachDist, float ForwardRadius, float DownwardRadius)
		: MaxLedgeHeight(MaxHeight), MinLedgeHeight(MinHeight), ReachDistance(ReachDist),
		  ForwardTraceRadius(ForwardRadius), DownwardTraceRadius(DownwardRadius)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float MaxLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float MinLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float ReachDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float ForwardTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
		float DownwardTraceRadius;
};



/**
 * 
 */
UCLASS(abstract)
class TRAILBLAZERCRISIS_API UTCStatics : public UObject
{
	GENERATED_BODY()
	
public:

	static FTransformAndComp ComponentLocalToWorld(const FTransformAndComp& LocalSpace);

	static FTransformAndComp ComponentWorldToLocal(const FTransformAndComp& WorldSpace);

	static FTransform AddTransforms(const FTransform& A, const FTransform& B);

	static FTransform SubTransforms(const FTransform& A, const FTransform& B);

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

	static FName CAMERA_MANAGER_ROT_CURVE;

	static FName CAMERA_MANAGER_DEBUG_CURVE;
};
