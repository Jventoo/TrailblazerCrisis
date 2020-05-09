// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TCStatics.h"
#include "MovementAction_NotifyState.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UMovementAction_NotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		EMovementAction MovementAction;
};
