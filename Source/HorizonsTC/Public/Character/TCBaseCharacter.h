// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Library/TCCharacterEnumLibrary.h"
#include "Library/TCCharacterStructLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Character/TCPlayerController.h"

#include "TCBaseCharacter.generated.h"

class UTimelineComponent;
class UAnimInstance;
class UAnimMontage;
class UTCCharacterAnimInstance;
class USoundCue;

/*
 * Base character class featuring advanced locomotion.
 */
UCLASS(BlueprintType)
class HORIZONSTC_API ATCBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATCBaseCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void PreInitializeComponents() override;

	virtual void Restart() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	/************************************************************************/
	/* Ragdoll System														*/
	/************************************************************************/

	/** Implement on BP to get required get up animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Ragdoll System")
		UAnimMontage* GetGetUpAnimation(bool bRagdollFaceUpState);

	UFUNCTION(BlueprintCallable, Category = "Ragdoll System")
		virtual void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll System")
		virtual void RagdollEnd();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll System")
		void RagdollOnDeath(bool Retrigger = true);

private:
	void DisablePhysicsSim();

	bool bSprintDisabled = false;


	/************************************************************************/
	/* Character States														*/
	/************************************************************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetMovementState(EMovementState NewState);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EMovementState GetMovementState() { return MovementState; }

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EMovementState GetPrevMovementState() { return PrevMovementState; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetMovementAction(EMovementAction NewAction);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EMovementAction GetMovementAction() { return MovementAction; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetStance(EStance NewStance);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EStance GetStance() { return Stance; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetRotationMode(ERotationMode NewRotationMode);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		ERotationMode GetRotationMode() { return RotationMode; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetGait(EGait NewGait);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EGait GetGait() { return Gait; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetViewMode(EViewMode NewViewMode);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EViewMode GetViewMode() { return ViewMode; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetOverlayState(EOverlayState NewState);

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EOverlayState GetOverlayState() { return OverlayState; }

	UFUNCTION(BlueprintGetter, Category = "Character States")
		EOverlayState SwitchRight() { return OverlayState; }

	UFUNCTION(BlueprintGetter, Category = "Character States")
		bool GetJumpJetsEnabled() { return bJumpJetsEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetJumpJetsEnabled(bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "Character States")
		void SetSprintDisabled(bool Disabled);


	/************************************************************************/
	/* Input																*/
	/************************************************************************/

	UFUNCTION(BlueprintGetter, Category = "Input")
		EStance GetDesiredStance() { return DesiredStance; }

	UFUNCTION(BlueprintSetter, Category = "Input")
		void SetDesiredStance(EStance NewStance) { DesiredStance = NewStance; }

	UFUNCTION(BlueprintGetter, Category = "Input")
		EGait GetDesiredGait() { return DesiredGait; }

	UFUNCTION(BlueprintSetter, Category = "Input")
		void SetDesiredGait(EGait NewGait) { DesiredGait = NewGait; }

	UFUNCTION(BlueprintGetter, Category = "Input")
		ERotationMode GetDesiredRotationMode() { return DesiredRotationMode; }

	UFUNCTION(BlueprintSetter, Category = "Input")
		void SetDesiredRotationMode(ERotationMode NewRotMode) { DesiredRotationMode = NewRotMode; }

	UFUNCTION(BlueprintCallable, Category = "Input")
		FVector GetPlayerMovementInput();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Input")
		void JumpJets();


	/************************************************************************/
	/* Rotation System														*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Rotation System")
		void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);


	/************************************************************************/
	/* Mantle																*/
	/************************************************************************/

	/** Implement on BP to get correct mantle parameter set according to character state */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Mantle System")
		FMantleAsset GetMantleAsset(EMantleType MantleType);

	UFUNCTION(BlueprintCallable, Category = "Mantle System")
		virtual bool MantleCheckGrounded();

	UFUNCTION(BlueprintCallable, Category = "Mantle System")
		virtual bool MantleCheckFalling();

	
	/************************************************************************/
	/* Movement System														*/
	/************************************************************************/

	UFUNCTION(BlueprintGetter, Category = "Movement System")
		bool HasMovementInput() { return bHasMovementInput; }

	UFUNCTION(BlueprintCallable, Category = "Movement System")
		void SetHasMovementInput(bool bNewHasMovementInput);

	UFUNCTION(BlueprintCallable, Category = "Movement System")
		FMovementSettings GetTargetMovementSettings();

	UFUNCTION(BlueprintCallable, Category = "Movement System")
		EGait GetAllowedGait();

	UFUNCTION(BlueprintCallable, Category = "Movement States")
		EGait GetActualGait(EGait AllowedGait);

	UFUNCTION(BlueprintCallable, Category = "Movement System")
		bool CanSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement System")
		bool CanPlayerJump();

	/** BP implementable function that called when Breakfall starts */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement System")
		void OnBreakfall();
		virtual void OnBreakfall_Implementation();

	/** BP implementable function that called when Roll starts */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement System")
		void OnRoll();
		virtual void OnRoll_Implementation();

	/** Implement on BP to get required roll animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Movement System")
		UAnimMontage* GetRollAnimation();

	
	/************************************************************************/
	/* Utility																*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Utility")
		float GetAnimCurveValue(FName CurveName);

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		UMeshComponent* CurrentHeldObject;

	/** Implement on BP to update held objects */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HeldObject")
		void UpdateHeldObject();

	UFUNCTION(BlueprintCallable, Category = "HeldObject")
		void ClearHeldObject();

	UFUNCTION(BlueprintCallable, Category = "HeldObject")
		void AttachToHand(UStaticMeshComponent* NewStaticMesh, USkeletalMeshComponent* NewSkeletalMesh,
			class UClass* NewAnimClass, bool bLeftHand, FVector Offset);

	/** Implement on BP to update animation states of held objects */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Equipment")
		void UpdateHeldObjectAnimations();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
		ATCPlayerController* GetPlayerController() const;


	/************************************************************************/
	/* Camera																*/
	/************************************************************************/

	UFUNCTION(BlueprintGetter, Category = "Camera System")
	bool IsRightShoulder() { return bRightShoulder; }

	UFUNCTION(BlueprintCallable, Category = "Camera System")
	void SetRightShoulder(bool bNewRightShoulder) { bRightShoulder = bNewRightShoulder; }

	UFUNCTION(BlueprintCallable, Category = "Camera System")
	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius);

	UFUNCTION(BlueprintCallable, Category = "Camera System")
	virtual FTransform GetThirdPersonPivotTarget();

	UFUNCTION(BlueprintCallable, Category = "Camera System")
	virtual FVector GetFirstPersonCameraTarget();

	UFUNCTION(BlueprintCallable, Category = "Camera System")
	void GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut);


	/************************************************************************/
	/* Essentials															*/
	/************************************************************************/

	UFUNCTION(BlueprintGetter, Category = "Essential Information")
		FVector GetAcceleration() { return Acceleration; }

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void SetAcceleration(const FVector& NewAcceleration);

	UFUNCTION(BlueprintGetter, Category = "Essential Information")
		bool IsMoving() { return bIsMoving; }

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void SetIsMoving(bool bNewIsMoving);

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		FVector GetMovementInput();

	UFUNCTION(BlueprintGetter, Category = "Essential Information")
		float GetMovementInputAmount() { return MovementInputAmount; }

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void SetMovementInputAmount(float NewMovementInputAmount);

	UFUNCTION(BlueprintGetter, Category = "Essential Information")
		float GetSpeed() { return Speed; }

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void SetSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		FRotator GetAimingRotation() { return GetControlRotation(); }

	UFUNCTION(BlueprintGetter, Category = "Essential Information")
		float GetAimYawRate() { return AimYawRate; }

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void SetAimYawRate(float NewAimYawRate);

	UFUNCTION(BlueprintCallable, Category = "Essential Information")
		void GetControlForwardRightVector(FVector& Forward, FVector& Right);


	/************************************************************************/
	/* Health & Damage														*/
	/************************************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		uint32 bIsDying : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
		float CurrentHealth = 100.0f;


	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float NewHealth);

	virtual float TakeDamage(float Dmg, const FDamageEvent& DmgEvent, AController* EventInstigator, AActor* DmgCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void KilledBy(APawn* EventInstigator);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
		bool CanDie();

	UFUNCTION(BlueprintCallable, Category = "Health")
		bool Die(float KillingDmg, const FDamageEvent& DmgEvent, AController* Killer, AActor* DmgCauser);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		UAnimMontage* DeathAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		USoundCue* DeathSound;

	virtual void PlayHit(float DmgTaken, const FDamageEvent& DmgEvent, APawn* EventInstigator, AActor* DmgCauser);

	virtual void OnDeath(float KillingDmg, const FDamageEvent& DmgEvent, APawn* EventInstigator, AActor* DmgCauser);

	void StopAllAnimMontages();

	/** Ragdoll System */

	void RagdollUpdate();

	void SetActorLocationDuringRagdoll();

	/** Stace Changes */

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnMovementStateChanged(EMovementState PreviousState);

	virtual void OnMovementActionChanged(EMovementAction PreviousAction);

	virtual void OnStanceChanged(EStance PreviousStance);

	virtual void OnRotationModeChanged(ERotationMode PreviousRotationMode);

	virtual void OnGaitChanged(EGait PreviousGait);

	virtual void OnViewModeChanged(EViewMode PreviousViewMode);

	virtual void OnOverlayStateChanged(EOverlayState PreviousState);

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void OnJumped_Implementation() override;

	void Landed(const FHitResult& Hit) override;

	void OnLandFrictionReset();

	void SetEssentialValues(float DeltaTime);

	void UpdateCharacterMovement();

	void UpdateDynamicMovementSettings(EGait AllowedGait);

	void UpdateGroundedRotation(float DeltaTime);

	void UpdateInAirRotation(float DeltaTime);

	/** Mantle System */

	virtual void MantleStart(float MantleHeight, const FComponentAndTransform& MantleLedgeWS, EMantleType MantleType);

	virtual bool MantleCheck(const FMantleTraceSettings& TraceSettings,
	                         EDrawDebugTrace::Type DebugType = EDrawDebugTrace::Type::ForOneFrame);

	UFUNCTION()
	virtual void MantleUpdate(float BlendIn);
	
	UFUNCTION(BlueprintCallable, Category = Debug)
	virtual void MantleEnd();

	bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation,
	                         float HeightOffset, float RadiusOffset, EDrawDebugTrace::Type DebugType);

	/** Utils */

	float GetMappedSpeed();

	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);

	float CalculateGroundedRotationRate();

	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	void SetMovementModel();

	/** Input */

	void PlayerForwardMovementInput(float Value);

	void PlayerRightMovementInput(float Value);

	void PlayerCameraUpInput(float Value);

	void PlayerCameraRightInput(float Value);

	void JumpPressedAction();

	void JumpReleasedAction();

	void SprintPressedAction();

	void SprintReleasedAction();

	void AimPressedAction();

	void AimReleasedAction();

	void CameraPressedAction();

	void CameraReleasedAction();

	void OnSwitchCameraMode();

	void StancePressedAction();

	void RollPressedAction();

	void WalkPressedAction();

	void RagdollPressedAction();

	void VelocityDirectionPressedAction();

	void LookingDirectionPressedAction();

	virtual void StopWeaponFire();

public:
	UFUNCTION(BlueprintCallable, Category = Combat)
		void SetIsArmed(bool NewArmed);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool IsArmed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool HasWeaponEquipped() const;

protected:

	/************************************************************************/
	/* Footsteps															*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
		bool bPlayFootsteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
		float FootstepsVolume;

	/** Input */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
		ERotationMode DesiredRotationMode = ERotationMode::LookingDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
		EGait DesiredGait = EGait::Running;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
		EStance DesiredStance = EStance::Standing;

	UPROPERTY(EditDefaultsOnly, Category = "Input", BlueprintReadOnly)
		float LookUpDownRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Input", BlueprintReadOnly)
		float LookLeftRightRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Input", BlueprintReadOnly)
		float ViewModeSwitchHoldTime = 0.2f;

	UPROPERTY(Category = "Input", BlueprintReadOnly)
		int32 TimesPressedStance = 0;

	UPROPERTY(Category = "Input", BlueprintReadOnly)
		bool bBreakFall = false;

	UPROPERTY(Category = "Input", BlueprintReadOnly)
		bool bSprintHeld = false;

	UPROPERTY(Category = "Input", BlueprintReadOnly)
		bool bIsArmed = false;

	UPROPERTY(EditDefaultsOnly, Category = "Input", BlueprintReadOnly)
		bool bHasJumpJets = false;

	UPROPERTY(Category = "Input", BlueprintReadWrite)
		bool bJumpJetsEnabled;

	UPROPERTY(Category = "Input", BlueprintReadWrite)
		bool bJumpJetsOnCooldown = false;


	/** Camera System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera System")
	float ThirdPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera System")
	float FirstPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera System")
	bool bRightShoulder = false;

	/** State Values */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EOverlayState OverlayState = EOverlayState::Default;

	/** Movement System */

	UPROPERTY(BlueprintReadOnly, Category = "Movement System")
	FMovementSettings CurrentMovementSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement System")
	FDataTableRowHandle MovementModel;

	/** Mantle System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mantle System")
	FMantleTraceSettings GroundedTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mantle System")
	FMantleTraceSettings AutomaticTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mantle System")
	FMantleTraceSettings FallingTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mantle System")
	UCurveFloat* MantleTimelineCurve;

	/** Components */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* MantleTimeline = nullptr;

	/** Essential Information */

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		float MovementInputAmount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
		float AimYawRate = 0.0f;

	/** State Values */

	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EMovementState MovementState = EMovementState::None;

	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EMovementState PrevMovementState = EMovementState::None;

	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EMovementAction MovementAction = EMovementAction::None;

	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	ERotationMode RotationMode = ERotationMode::LookingDirection;

	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EGait Gait = EGait::Walking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EStance Stance = EStance::Standing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EViewMode ViewMode = EViewMode::ThirdPerson;

	/** Movement System */

	UPROPERTY(BlueprintReadOnly, Category = "Movement System")
	FMovementStateSettings MovementData;

	/** Rotation System */

	UPROPERTY(BlueprintReadOnly, Category = "Rotation System")
	FRotator TargetRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation System")
	FRotator InAirRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation System")
	float YawOffset = 0.0f;

	/** Mantle System */

	UPROPERTY(BlueprintReadOnly, Category = "Mantle System")
	FMantleParams MantleParams;

	UPROPERTY(BlueprintReadOnly, Category = "Mantle System")
	FComponentAndTransform MantleLedgeLS;

	UPROPERTY(BlueprintReadOnly, Category = "Mantle System")
	FTransform MantleTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Mantle System")
	FTransform MantleActualStartOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Mantle System")
	FTransform MantleAnimatedStartOffset;

	/** Ragdoll System */

	UPROPERTY(BlueprintReadOnly, Category = "Ragdoll System")
	bool bRagdollOnGround = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ragdoll System")
	bool bRagdollFaceUp = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ragdoll System")
	FVector LastRagdollVelocity;

	/** Cached Variables */

	FVector PreviousVelocity;

	float PreviousAimYaw = 0.0f;

	UTCCharacterAnimInstance* MainAnimInstance = nullptr;

	/** Last time the camera action button is pressed */
	float CameraActionPressedTime = 0.0f;

	/* Timer to manage camera mode swap action */
	FTimerHandle OnCameraModeSwapTimer;

	/* Timer to manage reset of braking friction factor after on landed event */
	FTimerHandle OnLandedFrictionResetTimer;
};
