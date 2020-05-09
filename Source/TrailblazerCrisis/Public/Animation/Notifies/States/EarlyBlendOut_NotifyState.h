// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TCStatics.h"
#include "EarlyBlendOut_NotifyState.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UEarlyBlendOut_NotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    UEarlyBlendOut_NotifyState();

    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, 
        UAnimSequenceBase* Animation, float FrameDeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        class UAnimMontage* Montage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        float BlendOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool CheckMovementState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        EMovementState MovementStateEq;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool CheckStance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        EStance StanceEq;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool CheckMovementInput;
};
