// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StepNotify.generated.h"

UENUM(BlueprintType)
enum class EFootstepType : uint8
{
    Step,
    WalkRun,
    Jump,
    Land
};

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UStepNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

    UStepNotify();

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        float VolumeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        float PitchMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        bool OverrideMaskCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        EFootstepType FootstepType;
};
