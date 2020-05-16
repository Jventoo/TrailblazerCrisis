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

#include "Interfaces/AnimationInterface.h"

#include "Engine.h"

// Sets default values
ATCCharacterBase::ATCCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	Tags.Add(TEXT("TC_Character"));

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

	DesiredRotMode = ERotationMode::LookingDirection;
	DesiredGait = EGait::Running;
	ViewMode = EViewMode::ThirdPerson;
	OverlayState = EOverlayState::Default;
	DesiredStance = EStance::Standing;

	// Deprecated
	bIsCrouching = bIsSprinting = bIsJumping = false;

	// Input
	LookUpRate = TurnRate = 1.25f;
	bBreakFall = bIsSprinting = bIsWalking = false;

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
	SetGait_Implementation(DesiredGait);
	SetRotationMode_Implementation(DesiredRotMode);
	SetViewMode_Implementation(ViewMode);
	SetOverlayState_Implementation(OverlayState);

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

void ATCCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ResetBrakingFrictionHandle);

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetEssentialValues();

	switch (MovementState)
	{
	case EMovementState::Grounded:
		UpdateCharacterMovement();
		UpdateGroundedRotation();
		break;

	case EMovementState::InAir:
		UpdateInAirRotation();

		if (HasMovementInput)
			MantleCheck(FallingTraceSettings);
		break;

	case EMovementState::Ragdoll:
		RagdollUpdate();
		break;

	default:
		break;
	}

	CacheValues();
}

// Called to bind functionality to input
void ATCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Binding BaseCharInput"));

	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &ATCCharacterBase::Jump);
	PlayerInputComponent->BindAction("Vault", IE_Released, this, &ATCCharacterBase::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATCCharacterBase::ToggleCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATCCharacterBase::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATCCharacterBase::StopSprinting);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ATCCharacterBase::StartWalking);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ATCCharacterBase::StopWalking);

	PlayerInputComponent->BindAction("CameraAction", IE_Released, this, &ATCCharacterBase::ChangeCameraView);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATCCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATCCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &ATCCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATCCharacterBase::LookUpAtRate);

	// Potentially have key for changing rotation mode
}

void ATCCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	switch (GetCharacterMovement()->MovementMode)
	{
	case EMovementMode::MOVE_Walking:	
	case EMovementMode::MOVE_NavWalking:
		ICharacterInterface::Execute_SetMovementState(this, EMovementState::Grounded);
		break;

	case EMovementMode::MOVE_Falling:
		ICharacterInterface::Execute_SetMovementState(this, EMovementState::InAir);
		break;
	}
}

void ATCCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bBreakFall)
		PlayBreakfall();
	else
	{
		auto CharMove = GetCharacterMovement();

		if (HasMovementInput)
			CharMove->BrakingFrictionFactor = 0.5f;
		else
			CharMove->BrakingFrictionFactor = 3.0f;

		GetWorld()->GetTimerManager().ClearTimer(ResetBrakingFrictionHandle);
		GetWorld()->GetTimerManager().SetTimer(ResetBrakingFrictionHandle, this,
			&ATCCharacterBase::ResetBrakingFriction, 0.5f, false);
	}
}

void ATCCharacterBase::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	// Set in air rotation to velocity rotation if we're moving
	InAirRotation = (Speed > 100.0f) ? LastVelocityRotation : GetActorRotation();

	if (MeshAnimInst)
		IAnimationInterface::Execute_Jumped(MeshAnimInst);
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

void ATCCharacterBase::StopWeaponFire()
{
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

	// Visibility Channel
	return ETraceTypeQuery::TraceTypeQuery1;
}

void ATCCharacterBase::ChangeCameraView()
{
	// If held, go to FP/TP

	// If tapped, change shoulder
	bRightShoulder = !bRightShoulder;
}


APlayerControllerBase* ATCCharacterBase::GetPlayerController() const
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	return PC;
}


void ATCCharacterBase::UpdateGroundedRotation()
{
	switch (MovementAction)
	{
	case EMovementAction::None:
	{
		if (CanUpdateMovingRotation())
		{
			switch (RotationMode)
			{
			case ERotationMode::VelocityDirection:
				
				SmoothCharacterRotation(FRotator(0.0f, LastVelocityRotation.Yaw, 0.0f), 800.0f, CalculateGroundedRotationRate());
				break;

			case ERotationMode::LookingDirection:

				if (Gait == EGait::Sprinting)
				{
					SmoothCharacterRotation(FRotator(0.0f, LastVelocityRotation.Yaw, 0.0f), 500.0f, CalculateGroundedRotationRate());
				}
				else
				{
					SmoothCharacterRotation(
						FRotator(0.0f, GetControlRotation().Yaw + GetAnimCurveValue(TEXT("YawOffset")), 0.0f),
						500.0f, CalculateGroundedRotationRate());
				}

				break;

			case ERotationMode::Aiming:
				
				SmoothCharacterRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f), 1000.0f, 20.0f);
				break;
			}
		}
		else // Not Moving
		{
			if (ViewMode == EViewMode::FirstPerson
				|| (ViewMode == EViewMode::ThirdPerson && RotationMode == ERotationMode::Aiming))
			{
				LimitRotation(-100.0f, 100.0f, 20.0f);
			}

			// Apply from rotation curve from TIP animations
			float CurveVal = GetAnimCurveValue(TEXT("RotationAmount"));

			if (FMath::Abs(CurveVal) > 0.001)
			{
				// Divided by 30 for 30 FPS animations
				float DeltaYaw = CurveVal * (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) / ( 1.0f / 30.0f));
				
				AddActorWorldRotation(FRotator(0.0f, DeltaYaw, 0.0f), false, false);
				
				TargetRotation = GetActorRotation();
			}
		}

		break;
	}

	case EMovementAction::Rolling:
	{
		if (HasMovementInput)
			SmoothCharacterRotation(FRotator(0.0f, LastMovementInputRotation.Yaw, 0.0f), 0.0f, 2.0f);

		break;
	}

	default:
		break;
	}
}

void ATCCharacterBase::UpdateInAirRotation()
{
	if (RotationMode == ERotationMode::Aiming)
	{
		SmoothCharacterRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f), 0.0f, 15.0f);
		InAirRotation = GetActorRotation();
	}
	else
	{
		SmoothCharacterRotation(FRotator(0.0f, InAirRotation.Yaw, 0.0f), 0.0f, 5.0f);
	}
}

void ATCCharacterBase::SmoothCharacterRotation(const FRotator& Target, float TargetInterpSpeed, float ActorInterpSpeed)
{
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(
		TargetRotation, Target, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), TargetInterpSpeed);

	SetActorRotation(
		UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), ActorInterpSpeed)
	);
}

void ATCCharacterBase::AddToCharacterRotation(const FRotator& DeltaRot)
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, DeltaRot);

	AddActorWorldRotation(DeltaRot);
}

void ATCCharacterBase::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed)
{
	auto ControlRot = GetControlRotation();
	auto DeltaRotYaw = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, GetActorRotation()).Yaw;

	if (!UKismetMathLibrary::InRange_FloatFloat(DeltaRotYaw, AimYawMin, AimYawMax))
	{
		float SmoothedYaw = (DeltaRotYaw > 0.0f) ? (ControlRot.Yaw + AimYawMin) : (ControlRot.Yaw + AimYawMax);

		SmoothCharacterRotation(FRotator(0.0f, SmoothedYaw, 0.0f), 0.0f, InterpSpeed);
	}
}

bool ATCCharacterBase::SetActorLocationAndRotationWithUpdate(const FVector& NewLocation, const FRotator& NewRotation, FHitResult SweepHitResult, bool bSweep, bool bTeleport)
{
	TargetRotation = NewRotation;
	auto Teleport = (bTeleport) ? ETeleportType::TeleportPhysics : ETeleportType::None;

	FHitResult* ptr = &SweepHitResult;

	return SetActorLocationAndRotation(NewLocation, NewRotation, bSweep, ptr, Teleport);
}

float ATCCharacterBase::CalculateGroundedRotationRate()
{
	float CurveVal = CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed());

	return CurveVal * UKismetMathLibrary::MapRangeClamped(AimYawRate, 0.0f, 300.0f, 1.0f, 3.0f);
}

bool ATCCharacterBase::CanUpdateMovingRotation()
{
	return ((IsMoving && HasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
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

		FHitResult HitInfo;
		SetActorLocationAndRotationWithUpdate(NewLoc, TargetRagdollRotation, HitInfo);
	}
	else
	{
		FHitResult HitInfo;
		SetActorLocationAndRotationWithUpdate(TargetRagdollLocation, TargetRagdollRotation, HitInfo);
	}
}

UAnimMontage* ATCCharacterBase::GetGetUpAnimation(bool RagdollFaceUp) const
{
	return nullptr;
}


bool ATCCharacterBase::MantleCheck(FMantleTraceSettings TraceSettings)
{
	FVector InitialTraceImpactPoint, InitialTraceNormal, DownTraceLocation;
	UPrimitiveComponent* HitComponent = nullptr;
	EMantleType MantleType;

	TArray<AActor*> IgnoreActors;

	auto Coll = UCollisionProfile::Get();
	ETraceTypeQuery Channel = Coll->ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);

	// Trace forward to find nonwalkable wall or obj
	auto MoveInput = GetPlayerMovementInput();

	FVector Start = GetCapsuleBaseLocation(2.0f) + (MoveInput * -30.0f)
		+ FVector(0, 0, (TraceSettings.MaxLedgeHeight + TraceSettings.MinLedgeHeight) / 2.0f);

	FVector End = Start + (MoveInput * TraceSettings.ReachDistance);
	float HalfHeight = (TraceSettings.MaxLedgeHeight - TraceSettings.MinLedgeHeight) / 2.0f + 1.0f;

	FHitResult ForwardHit;
	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), Start, End, TraceSettings.ForwardTraceRadius,
		HalfHeight, Channel, false, IgnoreActors, EDrawDebugTrace::None, ForwardHit, true);

	if (!GetCharacterMovement()->IsWalkable(ForwardHit) && ForwardHit.bBlockingHit && !ForwardHit.bStartPenetrating)
	{
		InitialTraceImpactPoint = ForwardHit.ImpactPoint;
		InitialTraceNormal = ForwardHit.ImpactNormal;
	}
	else
		return false;


	// Trace down from impact point and see if we can walk there
	End = (InitialTraceNormal * -15.0f) + FVector(InitialTraceImpactPoint.X, InitialTraceImpactPoint.Y, GetCapsuleBaseLocation(2.0f).Z);
	Start = End + FVector(0, 0, TraceSettings.MaxLedgeHeight + TraceSettings.DownwardTraceRadius + 1.0f);

	FHitResult DownHit;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, TraceSettings.DownwardTraceRadius, Channel,
		false, IgnoreActors, EDrawDebugTrace::None, DownHit, true);

	if (DownHit.bBlockingHit && GetCharacterMovement()->IsWalkable(DownHit))
	{
		HitComponent = DownHit.Component.Get();
		DownTraceLocation = FVector(DownHit.Location.X, DownHit.Location.Y, DownHit.ImpactPoint.Z);
	}
	else
		return false;

	// Check if capsule has room to mantle
	auto CapsuleLoc = GetCapsuleLocationFromBase(DownTraceLocation, 2.0f);
	FTransform TargetTransform;
	float MantleHeight = 0.0f;

	if (CapsuleHasRoomCheck(GetCapsuleComponent(), CapsuleLoc, 0.0f, 0.0f))
	{
		TargetTransform = FTransform(UKismetMathLibrary::Conv_VectorToRotator(InitialTraceNormal * FVector(-1.0, -1.0, 0.0)), CapsuleLoc);
		MantleHeight = (CapsuleLoc - GetActorLocation()).Z;
	}
	else
		return false;

	// Determine mantle type
	switch (MovementState)
	{
	case EMovementState::InAir:
		MantleType = EMantleType::FallingCatch;
		break;

	default:
		if (MantleHeight > 125.0f)
			MantleType = EMantleType::HighMantle;
		else
			MantleType = EMantleType::LowMantle;
	}

	// Start Mantle
	MantleStart(MantleHeight, FTransformAndComp(TargetTransform, HitComponent), MantleType);

	return true;
}

void ATCCharacterBase::MantleStart(float Height, FTransformAndComp MantleLedge, EMantleType Type)
{
	// Set mantle params
	auto MantleAsset = GetMantleAsset(Type);

	float PlayRate = UKismetMathLibrary::MapRangeClamped(Height, MantleAsset.LowHeight,
		MantleAsset.HighHeight, MantleAsset.LowPlayRate, MantleAsset.HighPlayRate);

	float StartPos = UKismetMathLibrary::MapRangeClamped(Height, MantleAsset.LowHeight,
		MantleAsset.HighHeight, MantleAsset.LowStartPosition, MantleAsset.HighStartPosition);

	MantleParams = FMantleParams(MantleAsset.AnimMontage, MantleAsset.PositionCorrectCurve, StartPos, PlayRate, MantleAsset.StartingOffset);

	// Convert component space
	MantleLedgeLS = UTCStatics::ComponentWorldToLocal(MantleLedge);

	// Calculate target and offset
	MantleTarget = MantleLedge.Transform;
	MantleActualStartOffset = UTCStatics::SubTransforms(GetActorTransform(), MantleTarget);

	// Calculate animated start offset
	FVector ATransLoc = UKismetMathLibrary::Conv_RotatorToVector(MantleTarget.Rotator()) * MantleParams.StartingOffset.Y;
	ATransLoc.Z = MantleParams.StartingOffset.Z;
	FTransform ATrans(MantleTarget.Rotator(), MantleTarget.GetLocation() - ATransLoc);
	MantleAnimatedStartOffset = UTCStatics::SubTransforms(ATrans, MantleTarget);

	// Clear char movement, set to mantling
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ICharacterInterface::Execute_SetMovementState(this, EMovementState::Mantling);

	ConfigureMantleTimeline();

	// Play anim montage
	if (MeshAnimInst && MantleParams.AnimMontage)
	{
		MeshAnimInst->Montage_Play(MantleParams.AnimMontage, MantleParams.PlayRate,
			EMontagePlayReturnType::MontageLength, MantleParams.StartingPosition, false);
	}
}

void ATCCharacterBase::MantleEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ATCCharacterBase::MantleUpdate(float BlendIn)
{
	MantleTarget = UTCStatics::ComponentLocalToWorld(MantleLedgeLS).Transform;

	auto CurveVect = MantleParams.PositionCorrectCurve->GetVectorValue(MantleParams.StartingPosition + GetMantlePlaybackPos());
	float PosAlpha = CurveVect.X;
	float XYCorrectionAlpha = CurveVect.Y;
	float ZCorrectionAlpha = CurveVect.Z;

	// Blend using the XY value from pos/correction curve
	FTransform BlendedXY = UKismetMathLibrary::TLerp(MantleActualStartOffset, 
		FTransform(MantleAnimatedStartOffset.Rotator(), 
			FVector(
				MantleAnimatedStartOffset.GetLocation().X, MantleAnimatedStartOffset.GetLocation().Y, MantleActualStartOffset.GetLocation().Z
			)
		), XYCorrectionAlpha);

	// Blend using the Z value from pos/correction curve
	FTransform BlendedZ = UKismetMathLibrary::TLerp(MantleActualStartOffset,
		FTransform(MantleActualStartOffset.Rotator(),
			FVector(
				MantleActualStartOffset.GetLocation().X, MantleActualStartOffset.GetLocation().Y, MantleAnimatedStartOffset.GetLocation().Z
			)
		), ZCorrectionAlpha);

	// Blend from currently blending transforms into final target
	FTransform BTrans = UKismetMathLibrary::TLerp(
		UTCStatics::AddTransforms(MantleTarget, FTransform(
			BlendedXY.GetRotation(), FVector(BlendedXY.GetLocation().X, BlendedXY.GetLocation().Y, BlendedZ.GetLocation().Z))),
		MantleTarget, PosAlpha);

	// Blend at the midpoint
	FTransform LerpedTarget = UKismetMathLibrary::TLerp(
		UTCStatics::AddTransforms(MantleTarget, MantleActualStartOffset), BTrans, BlendIn);

	// Set to lerped target
	FHitResult HitInfo;
	SetActorLocationAndRotationWithUpdate(LerpedTarget.GetLocation(), LerpedTarget.Rotator(), HitInfo);
}

bool ATCCharacterBase::CapsuleHasRoomCheck(UCapsuleComponent* Capsule, const FVector& TargetLoc, float HeightOffset, float RadiusOffset) const
{
	FHitResult OutHit;
	TArray<AActor*> IgnoreActors;

	float Z = Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere() + (RadiusOffset * -1.0f) + HeightOffset;

	auto Start = TargetLoc + FVector(0, 0, Z);
	auto End = TargetLoc - FVector(0, 0, Z);
	float Radius = Capsule->GetScaledCapsuleRadius() + RadiusOffset;

	UKismetSystemLibrary::SphereTraceSingleByProfile(GetWorld(), Start, End, Radius, TEXT("TC_Character"),
		false, IgnoreActors, EDrawDebugTrace::None, OutHit, true);

	return UKismetMathLibrary::BooleanNOR(OutHit.bBlockingHit, OutHit.bStartPenetrating);
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
		SetGait_Implementation(ActualGait);

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

	default:
	//case ERotationMode::Aiming:
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
				if (CanSprint())
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

		default:
			return false;
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

void ATCCharacterBase::PlayRoll()
{
	if (MeshAnimInst)
		MeshAnimInst->Montage_Play(GetRollAnimation(), 1.15f);
}


void ATCCharacterBase::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRate);// * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate);//BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::MoveForward(float Value)
{
	PlayerMovementInput(true);

	//if ((Controller != NULL) && (Value != 0.0f))
	//{
	//	// find out which way is forward
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);

	//	// get forward vector
	//	const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//	AddMovementInput(Dir, Value);
	//}
}


void ATCCharacterBase::MoveRight(float Value)
{
	PlayerMovementInput(false);

	//if ((Controller != NULL) && (Value != 0.0f))
	//{
	//	// find out which way is right
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);

	//	// get right vector 
	//	const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//	// add movement in that direction
	//	AddMovementInput(Dir, Value);
	//}
}


void ATCCharacterBase::ToggleCrouch()
{
	// Potentially add break fall mechanic where if hit in air, reduce damage taken
	// Add interaction with prone system

	if (MovementState == EMovementState::Grounded)
	{
		if (Stance == EStance::Standing)
		{
			DesiredStance = EStance::Crouching;
			Crouch();
		}
		else if (Stance == EStance::Standing)
		{
			DesiredStance = EStance::Standing;
			UnCrouch();
		}
	}

	/*if (!bIsCrouched)
	{
		bIsCrouched = true;
		GetCharacterMovement()->bWantsToCrouch = true;
	}
	else
	{
		bIsCrouched = false;
		GetCharacterMovement()->bWantsToCrouch = false;
	}*/
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

void ATCCharacterBase::Jump()
{
	// Only jump if we are not currently preoccupied with another action
	if (MovementAction == EMovementAction::None)
	{
		// If ragdolling, stop. Otherwise, try to jump if we're on the ground
		switch (MovementState)
		{
		case EMovementState::Ragdoll:
			RagdollEnd();
			break;

		case EMovementState::Mantling:
			return;

		default:
		{
			if (MovementState == EMovementState::Grounded)
			{
				// If we're moving and can mantle, mantle
				if (HasMovementInput)
				{
					if (MantleCheck(GroundedTraceSettings))
						return;
				}

				// If we can't mantle, take the character to a 'standing' stance
				switch (Stance)
				{
				case EStance::Prone:
					UnProne();
					break;

				case EStance::Crouching:
					UnCrouch();
					break;
					
				default:
					break;
				}

				// Finally, jump
				Super::Jump();
			}
			else if (MovementState == EMovementState::InAir)
			{
				// See if we can mantle
				MantleCheck(FallingTraceSettings);
			}

			break;
		}
		}
	}
}

void ATCCharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ATCCharacterBase::StartWalking()
{
	DesiredGait = EGait::Walking;

	bIsWalking = true;
}

void ATCCharacterBase::StopWalking()
{
	DesiredGait = EGait::Running;

	bIsWalking = false;
}


void ATCCharacterBase::Prone()
{
	if (CanCrouch())
	{
		bWantsToProne = true;
	}
}

void ATCCharacterBase::StartSprinting()
{
	StopWeaponFire();

	DesiredGait = EGait::Sprinting;

	bIsSprinting = true;
}


void ATCCharacterBase::StopSprinting()
{
	DesiredGait = EGait::Running;

	bIsSprinting = false;
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


bool ATCCharacterBase::GetIsSprinting() const
{
	return bIsSprinting;
}


void ATCCharacterBase::PlayerMovementInput(bool IsForwardAxis)
{
	if (MovementState == EMovementState::Grounded
		|| MovementState == EMovementState::InAir)
	{
		FVector ForwardVect, RightVect;
		GetControlVectors(ForwardVect, RightVect);

		float XOut = 0.0f, YOut = 0.0f;
		FixDiagonalGamepadValues(GetInputAxisValue(TEXT("MoveRight")), GetInputAxisValue(TEXT("MoveForward")), XOut, YOut);
		
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

	XOut = UKismetMathLibrary::FClamp(RangedClamped_Y * XIn, -1.0, 1.0);
	YOut = UKismetMathLibrary::FClamp(RangedClamped_X * YIn, -1.0, 1.0);
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

void ATCCharacterBase::PlayBreakfall()
{
	if (MeshAnimInst)
		MeshAnimInst->Montage_Play(GetRollAnimation(), 1.35f);
}

void ATCCharacterBase::ResetBrakingFriction()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}
