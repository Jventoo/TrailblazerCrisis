// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class TRAILBLAZERCRISIS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		bool bIsFiring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		bool bIsArmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Util)
		FName SkeletonName;

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Communication w quest manager and updates objective progress */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interactions, meta = (AllowPrivateAccess = "true"))
		class UObjectiveComponent* ObjectiveComp;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		class ABaseFirearm* CurrentWeapon;

	/** Measured in degrees */
	UPROPERTY(Transient)
		float Direction;

	/** Left-right movement clamped between [-1, 1] */
	UPROPERTY(Transient)
		float RightAxisValue;

	/** Fwd-bwd movemenet clamped between [-1, 1] */
	UPROPERTY(Transient)
		float ForwardAxisValue;

public:

	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Set private members and update movement component **/
	UFUNCTION(BlueprintCallable, Category = Movement)
		void ToggleCrouch();

	/** Update members and change camera **/
	UFUNCTION(BlueprintCallable, Category = Combat)
		void ToggleEquip();

	/** Return direction player is looking/moving **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetDirection() const;

	/** Return axis value corresponding to right-left movement between [-1, 1] **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetRightAxisVal(bool AbsoluteVal = false) const;

	/** Return axis value corresponding to fwd-back movement between [-1, 1] **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		float GetForwardAxisValue(bool AbsoluteVal = false) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool GetIsArmed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		bool GetIsSprinting() const;

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
	* Calculate movement/look direction from Fwd and Right axis values; called every tick.
	* @return  Float between [-180, 180] degrees
	*/
	UFUNCTION(BlueprintCallable, Category = Movement)
		float CalculateDirection(float ForwardValue, float RightValue);

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

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};