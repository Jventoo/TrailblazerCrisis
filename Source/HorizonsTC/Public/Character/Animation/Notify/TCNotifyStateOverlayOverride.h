// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TCNotifyStateOverlayOverride.generated.h"

/**
 * 
 */
UCLASS()
class HORIZONSTC_API UTCNotifyStateOverlayOverride : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration);
	
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);

	FString GetNotifyName_Implementation() const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	int32 OverlayOverrideState = 0;
};
