// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/PlayerControllerBase.h"
#include "TCCharacterBase.generated.h"

UCLASS()
class TRAILBLAZERCRISIS_API ATCCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATCCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	APlayerControllerBase* GetPlayerController() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/************************************************************************/
	/* Footsteps															*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
		bool bPlayFootsteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
		float FootstepsVolume;

	FORCEINLINE class UAudioComponent* GetFootstepsComp() const { return FootstepsComponent; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* FootstepsComponent;

	/************************************************************************/
	/* Input																*/
	/************************************************************************/

public:
	/** Return direction player is looking/moving **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetDirection() const;

	/** Return axis value corresponding to right-left movement between [-1, 1] **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetRightAxisVal(bool AbsoluteVal = false) const;

	/** Return axis value corresponding to fwd-back movement between [-1, 1] **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetForwardAxisValue(bool AbsoluteVal = false) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		bool GetIsSprinting() const;

	/** Set private members and update movement component **/
	UFUNCTION(BlueprintCallable, Category = Movement)
		void ToggleCrouch();

protected:

	/**
	 * Provide fwd-bwd input for non-root motion movement; update input vars for BP root motion.
	 * @param Value  Ranged between [-1, 1] where 1 is a maximum request forward
	 */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void MoveForward(float Value);

	/**
	 * Provide left-right input for non-root motion movement; update input vars for BP root motion.
	 * @param Value  Ranged between [-1, 1] where 1 is a maximum request right
	 */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void LookUpAtRate(float Rate);

	/**
	* Set Direction to NewDir
	* @param NewDir  Float between [-180, 180] degrees
	*/
	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetDirection(float NewDir);

	/** See ForwardAxisValue member declaration for use */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetRightAxisVal(float NewVal);

	/** See ForwardAxisValue member declaration for use */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetForwardAxisValue(float NewVal);

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsJumping;

	/** Measured in degrees */
	UPROPERTY(Transient)
		float Direction;

	/** Left-right movement clamped between [-1, 1] */
	UPROPERTY(Transient)
		float RightAxisValue;

	/** Fwd-bwd movemenet clamped between [-1, 1] */
	UPROPERTY(Transient)
		float ForwardAxisValue;
};
