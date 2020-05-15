// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CharacterInterface.h"
#include "Interfaces/CameraInterface.h"
#include "Engine/DataTable.h"

#include "Player/PlayerControllerBase.h"

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
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	APlayerControllerBase* GetPlayerController() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnJumped_Implementation() override;

protected:

	/************************************************************************/
	/* References															*/
	/************************************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
		UAnimInstance* MeshAnimInst;

	/************************************************************************/
	/* Character Stats														*/
	/************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		FVector Acceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		bool IsMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		bool HasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		FRotator LastVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		FRotator LastMovementInputRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		float MovementInputAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		float AimYawRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		FVector PreviousVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Stats")
		float PreviousAimYaw;

public:
	UFUNCTION(BlueprintCallable, Category = "Character Stats")
		void SetEssentialValues();

	UFUNCTION(BlueprintCallable, Category = "Character Stats")
		void CacheValues();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
		FVector CalculateAcceleration();

protected:

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

	virtual void StopWeaponFire();

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

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void ChangeCameraView();


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

	UFUNCTION(BlueprintCallable, Category = "Input")
		void PlayerMovementInput(bool IsForwardAxis);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
		FVector GetPlayerMovementInput() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
		void FixDiagonalGamepadValues(float XIn, float YIn, float& XOut, float& YOut) const;

	UFUNCTION(BlueprintCallable, Category = "Input")
		void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void Jump() override;

	virtual void StopJumping() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		void StartWalking();

	UFUNCTION(BlueprintCallable, Category = "Input")
		void StopWalking();

	UFUNCTION(BlueprintCallable, Category = "Input")
		void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Input")
		void StopSprinting();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Prone();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UnProne();

	void OnStartProne();

	void OnEndProne();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
		bool bWantsToProne;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
		bool GetIsSprinting() const;

protected:

	/**
	 * Provide fwd-bwd input for non-root motion movement; update input vars for BP root motion.
	 * @param Value  Ranged between [-1, 1] where 1 is a maximum request forward
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void MoveForward(float Value);

	/**
	 * Provide left-right input for non-root motion movement; update input vars for BP root motion.
	 * @param Value  Ranged between [-1, 1] where 1 is a maximum request right
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void LookUpAtRate(float Rate);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		ERotationMode DesiredRotMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		EGait DesiredGait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		EStance DesiredStance;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
		float LookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
		float TurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
		bool bBreakFall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		bool bIsSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		bool bIsWalking;

	// Deprecated
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		bool bIsCrouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		bool bIsJumping;

	// End Deprecated

	/************************************************************************/
	/* Rotation																*/
	/************************************************************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Rotation")
		void UpdateGroundedRotation();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		void UpdateInAirRotation();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		void SmoothCharacterRotation(const FRotator& Target, float TargetInterpSpeed, float ActorInterpSpeed);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		void AddToCharacterRotation(const FRotator& DeltaRot);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
		bool SetActorLocationAndRotationWithUpdate(const FVector& NewLocation, const FRotator& NewRotation,
			FHitResult SweepHitResult, bool bSweep = false, bool bTeleport = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rotation")
		float CalculateGroundedRotationRate();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rotation")
		bool CanUpdateMovingRotation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		FRotator InAirRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		float YawOffset;

	/************************************************************************/
	/* Ragdoll																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		virtual void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
		virtual void RagdollEnd();

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

	UFUNCTION(BlueprintCallable, Category = "Mantle")
		bool MantleCheck(FMantleTraceSettings TraceSettings);

	UFUNCTION(BlueprintCallable, Category = "Mantle")
		virtual void MantleStart(float Height, FTransformAndComp MantleLedge, EMantleType Type);

	UFUNCTION(BlueprintCallable, Category = "Mantle")
		virtual void MantleEnd();

	UFUNCTION(BlueprintCallable, Category = "Mantle")
		void MantleUpdate(float BlendIn);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, const FVector& TargetLoc, float HeightOffset, float RadiusOffset) const;

		// Blueprint Functions
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Mantle")
		void StopMantleTimeline();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintImplementableEvent, Category = "Movement")
		FMantleAsset GetMantleAsset(EMantleType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Movement")
		void ConfigureMantleTimeline();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintImplementableEvent, Category = "Movement")
		float GetMantlePlaybackPos() const;
		// End Blueprint Functions


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMantleParams MantleParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FTransformAndComp MantleLedgeLS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FTransform MantleTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FTransform MantleActualStartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FTransform MantleAnimatedStartOffset;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMantleTraceSettings GroundedTraceSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMantleTraceSettings AutomaticTraceSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMantleTraceSettings FallingTraceSettings;

	/************************************************************************/
	/* Movement																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetMovementModel();

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void UpdateCharacterMovement();

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void UpdateDynamicMovementSettings(EGait AllowedGait = EGait::Walking);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		FMovementSettings GetTargetMovementSettings() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		EGait GetAllowedGait() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		EGait GetActualGait(EGait AllowedGait = EGait::Walking) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		virtual bool CanSprint() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		float GetMappedSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		virtual class UAnimMontage* GetRollAnimation() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void PlayRoll();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FDataTableRowHandle MovementModel;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMovementSettings_State MovementData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FMovementSettings CurrentMovementSettings;

	/************************************************************************/
	/* Utility																*/
	/************************************************************************/

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		void GetControlVectors(FVector& ForwardV, FVector& RightV) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		FVector GetCapsuleBaseLocation(float ZOffset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		FVector GetCapsuleLocationFromBase(const FVector& BaseLoc, float ZOffset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		float GetAnimCurveValue(const FName& CurveName) const;

	/************************************************************************/
	/* Falling																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void PlayBreakfall();
	
private:
	FTimerHandle ResetBrakingFrictionHandle;

	UFUNCTION()
		void ResetBrakingFriction();
};
