// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TCStatics.h"
#include "AnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRAILBLAZERCRISIS_API IAnimationInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void Jumped();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetGroundedEntryState(EGroundedEntryState State);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetOverlayOverrideState(int32 OverlayOverrideState);
};
