// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TCStatics.h"
#include "PlayerCameraBehavior.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UPlayerCameraBehavior : public UAnimInstance
{
	GENERATED_BODY()

public:
    UPlayerCameraBehavior();

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Default")
        void UpdateCharacterInfo();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Refs")
        class APlayerController* PlayerController;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Refs")
        class APawn* ControlledPawn;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        EMovementState MovementState;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        EMovementAction MovementAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        ERotationMode RotationMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        EGait Gait;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        EStance Stance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        EViewMode ViewMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info")
        bool bRightShoulder;
};
