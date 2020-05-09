// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TCStatics.h"
#include "Engine/EngineTypes.h"
#include "CharacterInterface.generated.h"

USTRUCT(BlueprintType)
struct FCurrentStates
{
	GENERATED_USTRUCT_BODY()

	FCurrentStates() {}

	FCurrentStates(EMovementMode NewPMM, EMovementState NewState,
		EMovementState NewPrevState, EMovementAction NewAction,
		ERotationMode NewVelDir, EGait NewGait, EStance NewStance,
		EViewMode NewView, EOverlayState NewOverlay)
	{
		PawnMovementMode = NewPMM;
		MovementState = NewState;
		PrevMovementState = NewPrevState;
		MovementAction = NewAction;
		VelocityDir = NewVelDir;
		ActualGait = NewGait;
		ActualStance = NewStance;
		ViewMode = NewView;
		OverlayState = NewOverlay;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		TEnumAsByte<EMovementMode> PawnMovementMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EMovementState MovementState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EMovementState PrevMovementState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EMovementAction MovementAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		ERotationMode VelocityDir;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EGait ActualGait;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EStance ActualStance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EViewMode ViewMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Current States")
		EOverlayState OverlayState;

};

USTRUCT(BlueprintType)
struct FEssentialValues
{
	GENERATED_USTRUCT_BODY()

	FEssentialValues()
	{
		IsMoving = HasMovementInput = false;
		Speed = MovementInputAmt = AimYawRate = 0.0f;
	}

	FEssentialValues(const FVector& NewVelocity, const FVector& NewAcceleration, 
		const FVector& NewMovementInput, bool NewIsMoving, bool NewHasMI, float NewSpeed, 
		float NewMIAmt, const FRotator& NewAimRot, float NewYaw)
	{
		Velocity = NewVelocity;
		Acceleration = NewAcceleration;
		MovementInput = NewMovementInput;
		IsMoving = NewIsMoving;
		HasMovementInput = NewHasMI;
		Speed = NewSpeed;
		MovementInputAmt = NewMIAmt;
		AimingRotation = NewAimRot;
		AimYawRate = NewYaw;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		FVector Velocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		FVector Acceleration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		FVector MovementInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		bool IsMoving;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		bool HasMovementInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		float MovementInputAmt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		FRotator AimingRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Essential Values")
		float AimYawRate;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRAILBLAZERCRISIS_API ICharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/************************************************************************/
	/* Character Info														*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		FCurrentStates GetCurrentStates();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		FEssentialValues GetEssentialValues();

	/************************************************************************/
	/* Character States														*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetMovementState(EMovementState NewState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetMovementAction(EMovementAction NewAction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetRotationMode(ERotationMode NewRotMode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetGait(EGait NewGait);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetViewMode(EViewMode NewViewMode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetOverlayState(EOverlayState NewState);
};
