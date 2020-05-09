// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Camera/CameraShake.h"
#include "CameraShakeNotify.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UCameraShakeNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UCameraShakeNotify();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		TSubclassOf<class UCameraShake> ShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		float Scale;
};
