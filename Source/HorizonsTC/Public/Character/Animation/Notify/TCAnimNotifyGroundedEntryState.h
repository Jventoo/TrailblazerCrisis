// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Library/TCCharacterEnumLibrary.h"

#include "TCAnimNotifyGroundedEntryState.generated.h"

/**
 * 
 */
UCLASS()
class HORIZONSTC_API UTCAnimNotifyGroundedEntryState : public UAnimNotify
{
	GENERATED_BODY()

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	FString GetNotifyName_Implementation() const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	EGroundedEntryState GroundedEntryState = EGroundedEntryState::None;
};
