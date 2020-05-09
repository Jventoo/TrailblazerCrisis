// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Game/TC_PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Rotator.h"

#include "Game/PlayerCameraBehavior.h"
#include "Interfaces/CameraInterface.h"

ATC_PlayerCameraManager::ATC_PlayerCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CameraBehavior"));
	CameraBehavior->SetupAttachment(GetTransformComponent());
	CameraBehavior->bHiddenInGame = true;

	DebugViewOffset = FVector(350, 0, 50);
	DebugViewRotation = FRotator(-5.0f, 180.0f, 0.0f);
}

void ATC_PlayerCameraManager::OnPossess_Implementation(class APawn* NewPawn)
{
	ControlledPawn = NewPawn;

	auto AnimInst = CameraBehavior->GetAnimInstance();

	if (AnimInst)
	{
		auto AnimBP = Cast<UPlayerCameraBehavior>(AnimInst);

		if (AnimBP)
		{
			AnimBP->PlayerController = GetOwningPlayerController();
			AnimBP->ControlledPawn = ControlledPawn;
		}
	}
}

void ATC_PlayerCameraManager::CustomCameraBehavior(FVector& Loc, FRotator& Rot, float& FOV)
{
	if (ControlledPawn
		&& ControlledPawn->GetClass()->ImplementsInterface(UCameraInterface::StaticClass()))
	{
		FTransform PivotTarg;
		FVector FPTarg;
		float TPFOV = 0.0f, FPFOV = 0.0f;

		// Get Camera Params via interface
		PivotTarg = ICameraInterface::Execute_GetTPPivotTarget(ControlledPawn);
		FPTarg = ICameraInterface::Execute_GetFPCameraTarget(ControlledPawn);
		ICameraInterface::Execute_GetCameraParameters(ControlledPawn, TPFOV, FPFOV);

		// Calculate Target Camera Rot
		CalculateTargetCameraRot();

		// Calculate Smoothed Pivot Target
		CalculateSmoothedPivotTarg(PivotTarg);

		// Calculate Pivot Location
		CalculatePivotLoc();

		// Calculate Target Camera Loc
		CalculateTargetCameraLoc(PivotTarg);

		// Trace for an object blocking between camera and character
		TraceForBlockingObject();
	}
}

FVector ATC_PlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLoc, 
	FVector TargetLoc, FRotator CameraRot, FVector LagSpeeds)
{
	FRotator CameraRotLocal(0.0f, CameraRot.Yaw, 0.0f);

	FVector CurrentVect = CameraRotLocal.UnrotateVector(CurrentLoc);
	FVector TargetVect = CameraRotLocal.UnrotateVector(TargetLoc);

	float XComp = UKismetMathLibrary::FInterpTo(CurrentVect.X, TargetVect.X,
		UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), LagSpeeds.X);

	float YComp = UKismetMathLibrary::FInterpTo(CurrentVect.Y, TargetVect.Y,
		UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), LagSpeeds.Y);

	float ZComp = UKismetMathLibrary::FInterpTo(CurrentVect.Z, TargetVect.Z,
		UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), LagSpeeds.Z);

	return CameraRotLocal.RotateVector(FVector(XComp, YComp, ZComp));
}

float ATC_PlayerCameraManager::GetCameraBehaviorParams(FName CurveName)
{
	auto AnimInst = CameraBehavior->GetAnimInstance();

	if (AnimInst)
	{
		return AnimInst->GetCurveValue(CurveName);
	}
	else
		return 0.0f;
}

void ATC_PlayerCameraManager::CalculateTargetCameraRot()
{
	TargetCameraRotation = UKismetMathLibrary::RLerp(
		(UKismetMathLibrary::RInterpTo(
			GetCameraRotation(),
			GetOwningPlayerController()->GetControlRotation(),
			UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
			GetCameraBehaviorParams(UTCStatics::CAMERA_MANAGER_ROT_CURVE))),
		DebugViewRotation, GetCameraBehaviorParams(UTCStatics::CAMERA_MANAGER_DEBUG_CURVE), true);
}

void ATC_PlayerCameraManager::CalculateSmoothedPivotTarg(const FTransform& PivotTarg)
{
	FVector Lag(
		GetCameraBehaviorParams(TEXT("PivotLagSpeed_X")),
		GetCameraBehaviorParams(TEXT("PivotLagSpeed_Y")),
		GetCameraBehaviorParams(TEXT("PivotLagSpeed_Z")));

	SmoothedPivotTarget = FTransform(PivotTarg.GetRotation(),
		CalculateAxisIndependentLag(
			SmoothedPivotTarget.GetLocation(), PivotTarg.GetLocation(),
			TargetCameraRotation, Lag));
}

void ATC_PlayerCameraManager::CalculatePivotLoc()
{
	const auto& LocalSmoothedRot = SmoothedPivotTarget.Rotator();

	const auto& SecVect = (UKismetMathLibrary::GetForwardVector(LocalSmoothedRot)
		* GetCameraBehaviorParams(TEXT("PivotOffset_X")));

	const auto& ThirdVect = (UKismetMathLibrary::GetRightVector(LocalSmoothedRot)
		* GetCameraBehaviorParams(TEXT("PivotOffset_Y")));

	const auto& FourthVect = (UKismetMathLibrary::GetUpVector(LocalSmoothedRot)
		* GetCameraBehaviorParams(TEXT("PivotOffset_Z")));

	PivotLocation = SmoothedPivotTarget.GetLocation() + SecVect + ThirdVect + FourthVect;
}

void ATC_PlayerCameraManager::CalculateTargetCameraLoc(const FTransform& PivotTarg)
{
	const auto& SecVect = (UKismetMathLibrary::GetForwardVector(TargetCameraRotation)
		* GetCameraBehaviorParams(TEXT("CameraOffset_X")));

	const auto& ThirdVect = (UKismetMathLibrary::GetRightVector(TargetCameraRotation)
		* GetCameraBehaviorParams(TEXT("CameraOffset_Y")));

	const auto& FourthVect = (UKismetMathLibrary::GetUpVector(TargetCameraRotation)
		* GetCameraBehaviorParams(TEXT("CameraOffset_Z")));

	FVector AVect = PivotLocation + SecVect + ThirdVect + FourthVect;
	FVector BVect = PivotTarg.GetLocation() + DebugViewOffset;

	TargetCameraLocation =
		UKismetMathLibrary::VLerp(AVect, BVect,
			GetCameraBehaviorParams(UTCStatics::CAMERA_MANAGER_DEBUG_CURVE);
}

void ATC_PlayerCameraManager::TraceForBlockingObject()
{
	FVector Origin;
	float Radius;
	auto Channel = ICameraInterface::Execute_GetTPTraceParams(ControlledPawn, Origin, Radius);

	GetWorld()->SweepTestByChannel(Origin, TargetCameraLocation, Radius, )
}