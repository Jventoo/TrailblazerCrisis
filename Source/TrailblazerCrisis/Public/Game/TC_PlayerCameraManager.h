// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TC_PlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API ATC_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ATC_PlayerCameraManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CameraManager")
		void OnPossess(class APawn* NewPawn);

	UFUNCTION(BlueprintCallable, Category = "CameraManager")
		void CustomCameraBehavior(FVector& Loc, FRotator& Rot, float& FOV);

	UFUNCTION(BlueprintCallable, Category = "CameraManager")
		FVector CalculateAxisIndependentLag(FVector CurrentLoc, FVector TargetLoc,
			FRotator CameraRot, FVector LagSpeeds);

	UFUNCTION(BlueprintCallable, Category = "CameraManager")
		float GetCameraBehaviorParams(FName CurveName);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		class APawn* ControlledPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FVector DebugViewOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FRotator DebugViewRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FVector RootLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FTransform SmoothedPivotTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FVector PivotLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FVector TargetCameraLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
		FRotator TargetCameraRotation;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* CameraBehavior;

	void CalculateTargetCameraRot();

	void CalculateSmoothedPivotTarg(const FTransform& PivotTarg);

	void CalculatePivotLoc();

	void CalculateTargetCameraLoc(const FTransform& PivotTarg);

	void TraceForBlockingObject();
};
