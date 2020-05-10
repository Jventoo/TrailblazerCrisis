// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "CameraInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCameraInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRAILBLAZERCRISIS_API ICameraInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * Get information about our camera orientation
	 * @param TPFOV				Output parameter for current FOV of third person view
	 * @param FPFOV				Output parameter for current FOV of first person view
	 * @return bRightShoulder	Whether we are currently looking over the right shoulder (right = true, left = false)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		bool GetCameraParameters(float& TPFOV, float& FPFOV);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		FVector GetFPCameraTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		FTransform GetTPPivotTarget();

	/**
	 * Get information about our current third person trace parameters
	 * @param TraceOrigin		Vector origin of our trace
	 * @param TraceRadius		Output parameter for the radius of our spherical tracem
	 * @return TraceChannel		Output parameter what channel we'll be doing our trace from
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		ETraceTypeQuery GetTPTraceParams(FVector& TraceOrigin, float& TraceRadius);
};
