// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HumanoidAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRAILBLAZERCRISIS_API UHumanoidAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bReceivedInitDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bUseRootMotionValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		TArray<class UPhysicalMaterial*> FloorTypes;

private:
	class APawn* Owner;

	FTimerHandle UpdateReceivedHandle;

public:
	UHumanoidAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeUninitializeAnimation() override;

private:

	UFUNCTION()
		void SetReceivedDirTrue();
};
