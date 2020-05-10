// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/PlayerControllerBase.h"
#include "Interfaces/CharacterInterface.h"
#include "Interfaces/CameraInterface.h"
#include "TCCharacterBase.generated.h"

UENUM(BlueprintType)
enum class EMantleType : uint8
{
	HighMantle,
	LowMantle,
	FallingCatch
};

UCLASS()
class TRAILBLAZERCRISIS_API ATCCharacterBase 
	: public ACharacter, public ICharacterInterface, public ICameraInterface
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

protected:

	/************************************************************************/
	/* References															*/
	/************************************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
		UAnimInstance* MeshAnimInst;

	/************************************************************************/
	/* Character Stats														*/
	/************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		FVector Acceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		bool IsMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		bool HasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		FRotator LastVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		FRotator LastMovementInputRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		float MovementInputAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		float AimYawRate;

	/************************************************************************/
	/* Character States														*/
	/************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EMovementState MovementState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EMovementState PrevMovementState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EMovementAction MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		ERotationMode RotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EGait Gait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EStance Stance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats|States")
		EViewMode ViewMode;

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character States")
		FCurrentStates GetCurrentStates();
	virtual FCurrentStates GetCurrentStates_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character States")
		FEssentialValues GetEssentialValues();
	virtual FEssentialValues GetEssentialValues_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetMovementState(EMovementState NewState);
	virtual void SetMovementState_Implementation(EMovementState NewState) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetMovementAction(EMovementAction NewAction);
	virtual void SetMovementAction_Implementation(EMovementAction NewAction) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetRotationMode(ERotationMode NewRotMode);
	virtual void SetRotationMode_Implementation(ERotationMode NewRotMode) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetGait(EGait NewGait);
	virtual void SetGait_Implementation(EGait NewGait) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetViewMode(EViewMode NewViewMode);
	virtual void SetViewMode_Implementation(EViewMode NewViewMode) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void SetOverlayState(EOverlayState NewState);
	virtual void SetOverlayState_Implementation(EOverlayState NewState) override;

	UFUNCTION(BlueprintCallable, Category = "Character")
		void SetStance(EStance NewStance);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States")
		EOverlayState OverlayState;

	/************************************************************************/
	/* Camera																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		bool GetCameraParameters(float& TPFOV, float& FPFOV);
	virtual bool GetCameraParameters_Implementation(float& TPFOV, float& FPFOV) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		FVector GetFPCameraTarget();
	virtual FVector GetFPCameraTarget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		FTransform GetTPPivotTarget();
	virtual FTransform GetTPPivotTarget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		ETraceTypeQuery GetTPTraceParams(FVector& TraceOrigin, float& TraceRadius);
	virtual ETraceTypeQuery GetTPTraceParams_Implementation(
		FVector& TraceOrigin, float& TraceRadius) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float ThirdPersonFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float FirstPersonFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		bool bRightShoulder;

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

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UFUNCTION(BlueprintCallable, Category = Movement)
	void Prone();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void UnProne();

	void OnStartProne();

	void OnEndProne();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		bool bWantsToProne;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		ERotationMode DesiredRotMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		EGait DesiredGait;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		EStance DesiredStance;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		bool bBreakFall;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		bool bSprintHeld;

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

	/************************************************************************/
	/* Rotation																*/
	/************************************************************************/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FRotator InAirRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float YawOffset;

	/************************************************************************/
	/* Ragdoll																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		void RagdollEnd();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		void RagdollUpdate();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		void SetActorLocationDuringRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		virtual UAnimMontage* GetGetUpAnimation(bool RagdollFaceUp) const;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ragdoll")
		bool bRagdollOnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ragdoll")
		bool bRagdollFaceUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ragdoll")
		FVector LastRagdollVelocity;

	/************************************************************************/
	/* Mantle																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Mantle")
		void StopMantleTimeline();
};
