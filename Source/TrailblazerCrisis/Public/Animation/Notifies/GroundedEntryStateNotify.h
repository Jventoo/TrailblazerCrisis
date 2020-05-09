// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TCStatics.h"
#include "GroundedEntryStateNotify.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UGroundedEntryStateNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        EGroundedEntryState GroundedState;
};
