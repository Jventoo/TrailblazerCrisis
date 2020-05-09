// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "OverlayOverride_NotifyState.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UOverlayOverride_NotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UOverlayOverride_NotifyState();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		int32 OverrideState;
};
