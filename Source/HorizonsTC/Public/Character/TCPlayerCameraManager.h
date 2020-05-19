// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TCPlayerCameraManager.generated.h"

class ATCBaseCharacter;

/**
 * Player camera manager class
 */
UCLASS(Blueprintable, BlueprintType)
class HORIZONSTC_API ATCPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	ATCPlayerCameraManager();

public:
	UFUNCTION(BlueprintCallable)
	void OnPossess(ATCBaseCharacter* NewCharacter);

	UFUNCTION(BlueprintCallable)
	float GetCameraBehaviorParam(FName CurveName);

	/** Implement debug logic in BP */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DrawDebugTargets(FVector PivotTargetLocation);

protected:
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime);

	UFUNCTION(BlueprintCallable)
	FVector CalculateAxisIndependentLag(
		FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds, float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool CustomCameraBehavior(float DeltaTime, FVector& Location, FRotator& Rotation, float& FOV);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ATCBaseCharacter* ControlledCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* CameraBehavior = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName CustomTag = FName(TEXT("BM_Character"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector RootLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform SmoothedPivotTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector PivotLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TargetCameraLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetCameraRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FRotator DebugViewRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector DebugViewOffset;
};
