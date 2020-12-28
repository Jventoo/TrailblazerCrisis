// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Library/TCCharacterEnumLibrary.h"
#include "TCAnimNotifySetOverlay.generated.h"

/**
 * 
 */
UCLASS()
class HORIZONSTC_API UTCAnimNotifySetOverlay : public UAnimNotify
{
	GENERATED_BODY()

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	FString GetNotifyName_Implementation() const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
		EOverlayState NewOverlay = EOverlayState::Default;
};
