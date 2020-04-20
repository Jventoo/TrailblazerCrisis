// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCStatics.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UTCStatics : public UObject
{
	GENERATED_BODY()
	
public:
	// Do not edit. Tied to root motion
	static float MAX_MOVE_SPEED;
	
	// Do not edit. Tied to root motion
	static float SPRINT_MODIFIER;
};
