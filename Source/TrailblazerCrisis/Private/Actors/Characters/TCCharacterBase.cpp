// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Characters/TCCharacterBase.h"

#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Curves/CurveVector.h"

// Sets default values
ATCCharacterBase::ATCCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	auto charMove = GetCharacterMovement();
	charMove->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	charMove->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	charMove->JumpZVelocity = 600.f;
	charMove->AirControl = 0.2f;

	// Deprecated
	bIsCrouching = bIsSprinting = bIsJumping = false;
	ForwardAxisValue = RightAxisValue = Direction = 0.0f;

	// Input
	LookUpDownRate = LookLeftRightRate = 1.25f;
	bBreakFall = bSprintHeld = false;

	// Camera
	ThirdPersonFOV = FirstPersonFOV = 90.0f;
	bRightShoulder = false;

	// Stats
	IsMoving = HasMovementInput = false;
	Speed = MovementInputAmount = AimYawRate = PreviousAimYaw = 0.0f;

	// Footsteps
	bPlayFootsteps = true;
	FootstepsVolume = 1.0f;

	FootstepsComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepsComp"));
	FootstepsComponent->SetupAttachment(RootComponent);
	FootstepsComponent->bAutoActivate = false;

	// Ragdoll
	bRagdollOnGround = bRagdollFaceUp = false;

	// Movement
	CurrentMovementSettings = FMovementSettings(165, 350, 600, nullptr, nullptr);

	// Mantle
	GroundedTraceSettings = FMantleTraceSettings(250, 50, 75, 30, 30);
	AutomaticTraceSettings = FMantleTraceSettings(80, 40, 50, 30, 30);
	FallingTraceSettings = FMantleTraceSettings(150, 50, 70, 30, 30);
}

// Called when the game starts or when spawned
void ATCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Ensures mesh and it's anim instance update after our character
	GetMesh()->AddTickPrerequisiteActor(this);

	// Set anim instance
	MeshAnimInst = GetMesh()->GetAnimInstance();

	// Set movement model
	SetMovementModel();
	
	// Update movement states
	SetGait(DesiredGait);
	SetRotationMode(DesiredRotMode);
	SetViewMode(ViewMode);
	SetOverlayState(OverlayState);

	if (DesiredStance == EStance::Standing)
	{
		UnProne();
		UnCrouch();
	}
	else if (DesiredStance == EStance::Crouching)
		Crouch();
	else
		Prone();

	// Set default rotations
	TargetRotation = LastVelocityRotation = LastMovementInputRotation = GetActorRotation();
}

// Called every frame
void ATCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update root motion variables
	ForwardAxisValue = InputComponent->GetAxisValue("MoveForward");
	RightAxisValue = InputComponent->GetAxisValue("MoveRight");
}

// Called to bind functionality to input
void ATCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ATCCharacterBase::SetEssentialValues()
{
	Acceleration = CalculateAcceleration();

	auto LocVelocity = GetVelocity();
	Speed = FVector(LocVelocity.X, LocVelocity.Y, 0.0f).Size();

	IsMoving = (Speed > 1.0f);

	if (IsMoving)
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(LocVelocity);

	auto CharMove = GetCharacterMovement();

	MovementInputAmount = CharMove->GetCurrentAcceleration().Size() / CharMove->GetMaxAcceleration();
	
	HasMovementInput = (MovementInputAmount > 0.0f);

	if (HasMovementInput)
		LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(CharMove->GetCurrentAcceleration());

	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
}

void ATCCharacterBase::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}

FVector ATCCharacterBase::CalculateAcceleration()
{
	return (GetVelocity() - PreviousVelocity) / UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
}

FCurrentStates ATCCharacterBase::GetCurrentStates_Implementation()
{
	return FCurrentStates(GetCharacterMovement()->MovementMode,
		MovementState, PrevMovementState, MovementAction, RotationMode,
		Gait, Stance, ViewMode, OverlayState);
}

FEssentialValues ATCCharacterBase::GetEssentialValues_Implementation()
{
	return FEssentialValues(GetVelocity(), Acceleration,
		GetCharacterMovement()->GetCurrentAcceleration(), IsMoving, HasMovementInput,
		Speed, MovementInputAmount, GetControlRotation(), AimYawRate);
}

void ATCCharacterBase::SetMovementState_Implementation(EMovementState NewState)
{
	if (NewState != MovementState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;

		switch (MovementState)
		{

		case EMovementState::InAir:
		{
			switch (MovementAction)
			{
			case EMovementAction::None:

				InAirRotation = GetActorRotation();

				if (Stance == EStance::Crouching)
					UnCrouch();
				else if (Stance == EStance::Prone)
					UnProne();

				break;

			case EMovementAction::Rolling:

				RagdollStart();
				break;

			default:
				break;
			}

			break;
		}

		case EMovementState::Ragdoll:
		{
			if (PrevMovementState == EMovementState::Mantling)
				StopMantleTimeline();

			break;
		}

		default:
			break;
		}
	}
}

void ATCCharacterBase::SetMovementAction_Implementation(EMovementAction NewAction)
{
	if (MovementAction != NewAction)
	{
		auto PrevMovementAction = MovementAction;
		MovementAction = NewAction;

		if (MovementAction == EMovementAction::Rolling)
			Crouch();

		if (PrevMovementAction == EMovementAction::Rolling)
		{
			switch (DesiredStance)
			{
			case EStance::Crouching:
				Crouch();
				break;

			case EStance::Prone:
				Prone();
				break;

			case EStance::Standing:
				UnProne();
				UnCrouch();
				break;
			}
		}
	}
}

void ATCCharacterBase::SetRotationMode_Implementation(ERotationMode NewRotMode)
{
	if (RotationMode != NewRotMode)
	{
		RotationMode = NewRotMode;

		if (RotationMode == ERotationMode::VelocityDirection
			&& ViewMode == EViewMode::FirstPerson)
			ICharacterInterface::Execute_SetViewMode(this, EViewMode::ThirdPerson);
	}
}

void ATCCharacterBase::SetGait_Implementation(EGait NewGait)
{
	if (Gait != NewGait)
	{
		Gait = NewGait;
	}
}

void ATCCharacterBase::SetViewMode_Implementation(EViewMode NewViewMode)
{
	if (ViewMode != NewViewMode)
	{
		ViewMode = NewViewMode;

		if (ViewMode == EViewMode::FirstPerson)
		{
			if (RotationMode == ERotationMode::VelocityDirection)
				ICharacterInterface::Execute_SetRotationMode(this, ERotationMode::LookingDirection);
		}
		else
		{
			if (RotationMode != ERotationMode::Aiming)
				ICharacterInterface::Execute_SetRotationMode(this, DesiredRotMode);
		}
	}
}

void ATCCharacterBase::SetOverlayState_Implementation(EOverlayState NewState)
{
	if (OverlayState != NewState)
	{
		OverlayState = NewState;
	}
}

void ATCCharacterBase::SetStance(EStance NewStance)
{
	Stance = NewStance;
}


bool ATCCharacterBase::GetCameraParameters_Implementation(float& TPFOV, float& FPFOV)
{
	TPFOV = ThirdPersonFOV;
	FPFOV = FirstPersonFOV;

	return bRightShoulder;
}

FVector ATCCharacterBase::GetFPCameraTarget_Implementation()
{
	return GetMesh()->GetSocketLocation(UTCStatics::FP_CAMERA_SOCKET);
}

FTransform ATCCharacterBase::GetTPPivotTarget_Implementation()
{
	return GetActorTransform();
}

ETraceTypeQuery ATCCharacterBase::GetTPTraceParams_Implementation(
	FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = UTCStatics::DEFAULT_TP_TRACE_RADIUS;

	return ETraceTypeQuery::TraceTypeQuery1;
}


APlayerControllerBase* ATCCharacterBase::GetPlayerController() const
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	return PC;
}


void ATCCharacterBase::SetMovementModel()
{
	const FString ContextString = "Set Movement Model Context";
	MovementData = *MovementModel.GetRow<FMovementSettings_State>(ContextString);
}

void ATCCharacterBase::UpdateCharacterMovement()
{
	auto AllowedGait = GetAllowedGait();
	auto ActualGait = GetActualGait(AllowedGait);

	if (ActualGait != Gait)
		SetGait(ActualGait);

	UpdateDynamicMovementSettings(AllowedGait);
}

void ATCCharacterBase::UpdateDynamicMovementSettings(EGait AllowedGait)
{
	CurrentMovementSettings = GetTargetMovementSettings();

	auto CharMove = GetCharacterMovement();
	auto SwitchSpeed = 0.0f;

	switch (AllowedGait)
	{
	case EGait::Sprinting:
		SwitchSpeed = CurrentMovementSettings.SprintSpeed;
		break;

	case EGait::Walking:
		SwitchSpeed = CurrentMovementSettings.WalkSpeed;
		break;

	default:
		SwitchSpeed = CurrentMovementSettings.RunSpeed;
		break;
	}

	CharMove->MaxWalkSpeed = CharMove->MaxWalkSpeedCrouched = SwitchSpeed;

	auto VectorVal = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());

	CharMove->MaxAcceleration = VectorVal.X;
	CharMove->BrakingDecelerationWalking = VectorVal.Y;
	CharMove->GroundFriction = VectorVal.Z;
}

FMovementSettings ATCCharacterBase::GetTargetMovementSettings() const
{
	switch (RotationMode)
	{
	case ERotationMode::VelocityDirection:
	{
		switch (Stance)
		{
		case EStance::Standing:
			return MovementData.VelocityDirection.Standing;
			
		default:
			return MovementData.VelocityDirection.Crouching;
		}
		break;
	}

	case ERotationMode::LookingDirection:
	{
		switch (Stance)
		{
		case EStance::Standing:
			return MovementData.LookingDirection.Standing;

		default:
			return MovementData.LookingDirection.Crouching;
		}
		break;
	}

	case ERotationMode::Aiming:
	{
		switch (Stance)
		{
		case EStance::Standing:
			return MovementData.Aiming.Standing;

		default:
			return MovementData.Aiming.Crouching;
		}
		break;
	}
	}
}

EGait ATCCharacterBase::GetAllowedGait() const
{
	switch (Stance)
	{
	case EStance::Standing:
	{
		switch (RotationMode)
		{
		case ERotationMode::Aiming:
		{
			switch (DesiredGait)
			{
			case EGait::Walking:
				return EGait::Walking;

			default:
				return EGait::Running;
			}
		}

		default:
			switch (DesiredGait)
			{
			case EGait::Walking:
				return EGait::Walking;

			case EGait::Running:
				return EGait::Running;

			default:
				if (CanSprint)
					return EGait::Sprinting;
				else
					return EGait::Running;
			}
		}
	}

	case EStance::Crouching:
	{
		switch (DesiredGait)
		{
		case EGait::Walking:
			return EGait::Walking;

		default:
			return EGait::Running;
		}
	}

	default:
		return EGait::Walking;
	}
}

EGait ATCCharacterBase::GetActualGait(EGait AllowedGait) const
{
	float LocalWalk = CurrentMovementSettings.WalkSpeed;
	float LocalRun = CurrentMovementSettings.RunSpeed;
	float LocalSprint = CurrentMovementSettings.SprintSpeed;

	if (Speed >= (LocalRun + 10))
	{
		if (AllowedGait == EGait::Sprinting)
			return EGait::Sprinting;
		else
			return EGait::Running;
	}
	else if (Speed >= (LocalWalk + 10))
		return EGait::Running;
	else
		return EGait::Walking;
}

bool ATCCharacterBase::CanSprint() const
{
	if (HasMovementInput)
	{
		switch (RotationMode)
		{
		case ERotationMode::Aiming:
			return false;

		case ERotationMode::VelocityDirection:
			return MovementInputAmount > 0.9f;

		case ERotationMode::LookingDirection:
		{
			auto A = UKismetMathLibrary::Conv_VectorToRotator(GetCharacterMovement()->GetCurrentAcceleration());

			return MovementInputAmount > 0.9f 
				&& FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(A, GetControlRotation()).Yaw) < 50.0f;
		}
		}
	}
	else
		return false;
}

float ATCCharacterBase::GetMappedSpeed() const
{
	float LocalWalk = CurrentMovementSettings.WalkSpeed;
	float LocalRun = CurrentMovementSettings.RunSpeed;
	float LocalSprint = CurrentMovementSettings.SprintSpeed;

	if (Speed > LocalRun)
	{
		return UKismetMathLibrary::MapRangeClamped(Speed, LocalRun, LocalSprint, 2.0, 3.0);
	}
	else
	{
		if (Speed > LocalWalk)
		{
			return UKismetMathLibrary::MapRangeClamped(Speed, LocalWalk, LocalRun, 1.0, 2.0);
		}
		else
		{
			return UKismetMathLibrary::MapRangeClamped(Speed, 0.0, LocalWalk, 0.0, 1.0);
		}
	}
}

UAnimMontage* ATCCharacterBase::GetRollAnimation() const
{
	return nullptr;
}


void ATCCharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
	}
}


void ATCCharacterBase::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Dir, Value);
	}
}


void ATCCharacterBase::ToggleCrouch()
{
	if (!bIsCrouched)
	{
		bIsCrouched = true;
		GetCharacterMovement()->bWantsToCrouch = true;
	}
	else
	{
		bIsCrouched = false;
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}


void ATCCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EStance::Crouching);
}

void ATCCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EStance::Standing);
}


void ATCCharacterBase::Prone()
{
	if (CanCrouch())
	{
		bWantsToProne = true;
	}
}

void ATCCharacterBase::UnProne()
{
	bWantsToProne = false;
}

void ATCCharacterBase::OnStartProne()
{
	SetStance(EStance::Prone);
}

void ATCCharacterBase::OnEndProne()
{
	SetStance(EStance::Standing);
}


float ATCCharacterBase::GetDirection() const
{
	return Direction;
}

float ATCCharacterBase::GetRightAxisVal(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(RightAxisValue);
	else
		return RightAxisValue;
}


float ATCCharacterBase::GetForwardAxisValue(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(ForwardAxisValue);
	else
		return ForwardAxisValue;
}


bool ATCCharacterBase::GetIsSprinting() const
{
	return bIsSprinting;
}


void ATCCharacterBase::SetDirection(float NewDir)
{
	Direction = NewDir;
}

void ATCCharacterBase::SetRightAxisVal(float NewVal)
{
	RightAxisValue = NewVal;
}


void ATCCharacterBase::SetForwardAxisValue(float NewVal)
{
	ForwardAxisValue = NewVal;
}


void ATCCharacterBase::RagdollStart()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ICharacterInterface::Execute_SetMovementState(this, EMovementState::Ragdoll);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	auto LocMesh = GetMesh();
	LocMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	LocMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LocMesh->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, true);

	MeshAnimInst->StopAllMontages(0);
}

void ATCCharacterBase::RagdollEnd()
{
	MeshAnimInst->SavePoseSnapshot(TEXT("RagdollPose"));

	if (bRagdollOnGround)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		MeshAnimInst->Montage_Play(GetGetUpAnimation(bRagdollFaceUp));
	}
	else
	{
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	auto LocMesh = GetMesh();
	LocMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	LocMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LocMesh->SetAllBodiesSimulatePhysics(false);
}

void ATCCharacterBase::RagdollUpdate()
{
	auto LocMesh = GetMesh();

	LastRagdollVelocity = LocMesh->GetPhysicsLinearVelocity(TEXT("root"));

	LocMesh->SetAllMotorsAngularDriveParams(
		UKismetMathLibrary::MapRangeClamped(LastRagdollVelocity.Size(), 0, 1000, 0, 25000),
		0.0f, 0.0f);

	LocMesh->SetEnableGravity(LastRagdollVelocity.Z > -4000);

	SetActorLocationDuringRagdoll();
}

void ATCCharacterBase::SetActorLocationDuringRagdoll()
{
	auto LocMesh = GetMesh();
	auto SocketRot = LocMesh->GetSocketRotation(TEXT("pelvis"));

	FVector TargetRagdollLocation = LocMesh->GetSocketLocation(TEXT("pelvis"));

	bRagdollFaceUp = SocketRot.Roll < 0.0f;

	float Yaw = (bRagdollFaceUp) ? SocketRot.Yaw - 180 : SocketRot.Yaw;
	FRotator TargetRagdollRotation(0.0f, Yaw, 0.0f);

	FVector End(TargetRagdollLocation.X, TargetRagdollLocation.Y,
		TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FHitResult OutHit;

	GetWorld()->LineTraceSingleByChannel(OutHit, TargetRagdollLocation, End, 
		ECollisionChannel::ECC_Visibility);

	bRagdollOnGround = OutHit.bBlockingHit;

	if (bRagdollOnGround)
	{
		float Delta = GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
			- UKismetMathLibrary::Abs(OutHit.ImpactPoint.Z - OutHit.TraceStart.Z);

		FVector NewLoc(TargetRagdollLocation.X, TargetRagdollLocation.Y,
			(TargetRagdollLocation.Z + 2.0 + Delta));

		SetActorLocationAndRotation(NewLoc, TargetRagdollRotation);
	}
	else
		SetActorLocationAndRotation(TargetRagdollLocation, TargetRagdollRotation);
}

UAnimMontage* ATCCharacterBase::GetGetUpAnimation(bool RagdollFaceUp) const
{
	return nullptr;
}

void ATCCharacterBase::PlayerMovementInput(bool IsForwardAxis)
{
	if (MovementState == EMovementState::Grounded
		|| MovementState == EMovementState::InAir)
	{
		FVector ForwardVect, RightVect;
		GetControlVectors(ForwardVect, RightVect);

		float XOut = 0.0f, YOut = 0.0f;
		FixDiagonalGamepadValues(GetInputAxisValue(TEXT("MoveForward")), GetInputAxisValue(TEXT("MoveRight")), XOut, YOut);

		if (IsForwardAxis)
		{
			AddMovementInput(ForwardVect, YOut);
		}
		else
		{
			AddMovementInput(RightVect, XOut);
		}
	}
}

FVector ATCCharacterBase::GetPlayerMovementInput() const
{
	FVector Forward, Right;

	GetControlVectors(Forward, Right);

	Forward *= GetInputAxisValue(TEXT("MoveForward"));
	Right *= GetInputAxisValue(TEXT("MoveRight"));

	auto Final = Forward + Right;
	Final.Normalize(0.0001);

	return Final;
}

void ATCCharacterBase::FixDiagonalGamepadValues(float XIn, float YIn, float& XOut, float& YOut) const
{
	auto RangedClamped_X = UKismetMathLibrary::MapRangeClamped(FMath::Abs(XIn), 0, 0.6, 1, 1.2);
	auto RangedClamped_Y = UKismetMathLibrary::MapRangeClamped(FMath::Abs(YIn), 0, 0.6, 1, 1.2);

	XOut = UKismetMathLibrary::Clamp(RangedClamped_Y * XIn, -1.0, 1.0);
	YOut = UKismetMathLibrary::Clamp(RangedClamped_X * YIn, -1.0, 1.0);
}

void ATCCharacterBase::GetControlVectors(FVector& ForwardV, FVector& RightV) const
{
	float Yaw = GetControlRotation().Yaw;

	ForwardV = UKismetMathLibrary::GetForwardVector(FRotator(0, Yaw, 0));
	RightV = UKismetMathLibrary::GetRightVector(FRotator(0, Yaw, 0));
}

FVector ATCCharacterBase::GetCapsuleBaseLocation(float ZOffset) const
{
	auto Capsule = GetCapsuleComponent();

	auto Multiplier = Capsule->GetScaledCapsuleHalfHeight() + ZOffset;

	return Capsule->GetComponentLocation() - (Capsule->GetUpVector() * Multiplier);
}

FVector ATCCharacterBase::GetCapsuleLocationFromBase(const FVector& BaseLoc, float ZOffset) const
{
	return BaseLoc + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + ZOffset);
}

float ATCCharacterBase::GetAnimCurveValue(const FName& CurveName) const
{
	if (MeshAnimInst)
		return MeshAnimInst->GetCurveValue(CurveName);
	else
		return 0.0f;
}
