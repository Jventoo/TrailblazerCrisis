// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Player/PlayerControllerBase.h"
#include "Actors/Weapons/BaseFirearm.h"
#include "Animation/HumanoidAnimInstance.h"

#include "Engine.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arms

	// Create components for holding objects
	HeldObjectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectRoot"));
	HeldObjectRoot->SetupAttachment(GetMesh());

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(HeldObjectRoot);
	SkeletalMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshComp->bHiddenInGame = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(HeldObjectRoot);
	StaticMeshComp->SetCollisionProfileName(TEXT("TC_Prop"));
	StaticMeshComp->bHiddenInGame = true;


	bIsFiring = false;
	bIsAiming = false;
	bIsArmed = false;

	bSpawnWeapon = true;
	
	CurrentWeapon = nullptr;

	PrimaryActorTick.bCanEverTick = true;
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn our weapon
	//if (bSpawnWeapon && WeaponClass)
	//{
	//	FActorSpawnParameters SpawnParams;
	//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//	CurrentWeapon = GetWorld()->SpawnActor<ABaseFirearm>(WeaponClass, SpawnParams);
	//}

	//// Attach our weapon to our character's back
	//if (CurrentWeapon)
	//{
	//	CurrentWeapon->SetOwningPawn(this);
	//	CurrentWeapon->AttachMeshToPawn(WeaponUnequipSocket);
	//}
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Binding PlayerCharInput"));

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnStopFire);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::OnStartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::OnStopAiming);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::OnReload);

	PlayerInputComponent->BindAction("Holster", IE_Pressed, this, &APlayerCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &APlayerCharacter::NextFireMode);
}


bool APlayerCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


//float APlayerCharacter::CalculateDirection(float ForwardValue, float RightValue)
//{
//	FVector InputVector(ForwardValue, (RightValue * -1), 0);
//	FRotator InputRot = InputVector.ToOrientationRotator();
//
//	FRotator CameraRot = FollowCamera->GetComponentTransform().GetRotation().Rotator();
//	FRotator CapsuleRot = GetCapsuleComponent()->GetComponentTransform().GetRotation().Rotator();
//
//	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(CameraRot, CapsuleRot);
//	FRotator FinalRot = UKismetMathLibrary::NormalizedDeltaRotator(DeltaRot, InputRot);
//
//	return FinalRot.Yaw;
//}


//////////////////////////////////////////////////////////////////////////
// Combat

void APlayerCharacter::ToggleEquip()
{
	if (!bIsArmed)
	{
		bIsArmed = true;

		if (CurrentWeapon)
			CurrentWeapon->BeginEquip(this);

		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
		{
			AnimInst->IsArmed = true;
		}

		//ICharacterInterface::Execute_SetOverlayState(this, EOverlayState::Rifle);

		//UpdateCombatCamera();
	}
	else
	{
		bIsArmed = false;

		if (CurrentWeapon)
			CurrentWeapon->BeginUnequip();

		if (bIsAiming)
			OnStopAiming();

		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
		{
			AnimInst->IsArmed = false;
		}

		//ICharacterInterface::Execute_SetOverlayState(this, EOverlayState::Default);

		//UpdateCombatCamera();
	}
}


bool APlayerCharacter::GetIsArmed() const
{
	return bIsArmed;
}


bool APlayerCharacter::CanReload() const
{
	return bIsArmed && !bIsSprinting && !bIsJumping;
}


bool APlayerCharacter::CanFire() const
{
	return bIsArmed && !bIsSprinting && !bIsJumping;
}


void APlayerCharacter::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


void APlayerCharacter::OnStartFire()
{
	if (GetIsSprinting())
	{
		StopSprinting();
	}

	StartWeaponFire();
}


void APlayerCharacter::OnStopFire()
{
	StopWeaponFire();
}


void APlayerCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
			bIsFiring = true;
		}
	}
}


void APlayerCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
			bIsFiring = false;
		}
	}
}


void APlayerCharacter::OnStartAiming()
{
	if (!bIsAiming && bIsArmed)
	{
		if (GetIsSprinting())
		{
			StopSprinting();
		}

		bIsAiming = true;
		//UpdateAimingFOV();
		GetPlayerController()->ToggleCrosshair(true);

		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
			AnimInst->IsAiming = true;

		GetCharacterMovement()->MaxWalkSpeed /= 2.0f;

		ICharacterInterface::Execute_SetRotationMode(this, ERotationMode::Aiming);
	}
}


void APlayerCharacter::OnStopAiming()
{
	if (bIsAiming)
	{
		bIsAiming = false;
		//UpdateAimingFOV();
		GetPlayerController()->ToggleCrosshair(false);

		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
			AnimInst->IsAiming = false;

		GetCharacterMovement()->MaxWalkSpeed *= 2.0f;

		if (ViewMode == EViewMode::FirstPerson)
			ICharacterInterface::Execute_SetRotationMode(this, ERotationMode::LookingDirection);
		else
			ICharacterInterface::Execute_SetRotationMode(this, DesiredRotMode);
	}
}


void APlayerCharacter::AddRecoil(float Pitch, float Yaw)
{
	Pitch *= AccuracyMultiplier;
	Yaw *= AccuracyMultiplier;

	if (!bIsAiming)
	{
		Pitch *= HipFirePenalty;
		Pitch *= HipFirePenalty;
	}

	AddControllerPitchInput(Pitch);
	AddControllerYawInput(Yaw);
}


void APlayerCharacter::NextFireMode()
{
	if (bIsArmed)
		CurrentWeapon->SwitchToNextFireMode();
}

bool APlayerCharacter::CanSprint() const
{
	return Super::CanSprint() && !IsFiring();
}

void APlayerCharacter::SetOverlayState_Implementation(EOverlayState NewState)
{
	Super::SetOverlayState_Implementation(NewState);

	UpdateHeldObject();
}

void APlayerCharacter::AttachToHand(UStaticMesh* StaticMesh, USkeletalMesh* SkeletalMesh, TSubclassOf<UAnimInstance> SkelMeshAnimInstance, bool bLeftHand, const FVector& Offset)
{
	ClearHeldObject();

	if (StaticMesh)
		StaticMeshComp->SetStaticMesh(StaticMesh);
	
	if (SkeletalMesh)
	{
		SkeletalMeshComp->SetSkeletalMesh(SkeletalMesh);

		if (SkelMeshAnimInstance)
			SkeletalMeshComp->SetAnimClass(SkelMeshAnimInstance);
	}

	HeldObjectRoot->AttachToComponent(GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		bLeftHand ? TEXT("VB LHS_ik_hand_gun") : TEXT("VB RHS_ik_hand_gun"));
	HeldObjectRoot->SetRelativeLocation(Offset);
}

void APlayerCharacter::ClearHeldObject()
{
	StaticMeshComp->SetStaticMesh(nullptr);
	SkeletalMeshComp->SetSkeletalMesh(nullptr, false);
	SkeletalMeshComp->SetAnimClass(nullptr);
}


// Unsmoothed version. Use timeline in BP for smoothed version
void APlayerCharacter::UpdateCombatCamera_Implementation()
{
	if (bIsArmed)
	{
		FVector Offset(.0f, CombatArmOffset, .0f);
		CameraBoom->AddLocalOffset(Offset);
	}
	else
	{
		FVector Offset(.0f, CombatArmOffset * -1, .0f);
		CameraBoom->AddLocalOffset(Offset);
	}
}

void APlayerCharacter::UpdateAimingFOV_Implementation()
{
	if (bIsAiming)
		FollowCamera->SetFieldOfView(DefaultFOV * AimFOVRatio);
	else
		FollowCamera->SetFieldOfView(DefaultFOV);
}


FVector APlayerCharacter::GetFPCameraTarget_Implementation()
{
	return GetMesh()->GetSocketLocation(UTCStatics::FP_CAMERA_SOCKET);
}

FTransform APlayerCharacter::GetTPPivotTarget_Implementation()
{
	TArray<FVector> AvgArray;
	AvgArray.Add(GetMesh()->GetSocketLocation(TEXT("head")));
	AvgArray.Add(GetMesh()->GetSocketLocation(TEXT("root")));

	return FTransform(GetActorRotation(), UKismetMathLibrary::GetVectorArrayAverage(AvgArray));
}

ETraceTypeQuery APlayerCharacter::GetTPTraceParams_Implementation(
	FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetMesh()->GetSocketLocation(
		bRightShoulder ? (TEXT("TP_CameraTrace_R")) : (TEXT("TP_CameraTrace_L")));

	TraceRadius = UTCStatics::DEFAULT_TP_TRACE_RADIUS + 5.0f;

	// Camera channel
	return ETraceTypeQuery::TraceTypeQuery2;
}

void APlayerCharacter::MantleStart(float Height, FTransformAndComp MantleLedge, EMantleType Type)
{
	Super::MantleStart(Height, MantleLedge, Type);

	if (Type != EMantleType::LowMantle)
		ClearHeldObject();
}

void APlayerCharacter::MantleEnd()
{
	Super::MantleEnd();

	UpdateHeldObject();
}

UAnimMontage* APlayerCharacter::GetRollAnimation() const
{
	switch (OverlayState)
	{
	case EOverlayState::Binos:
	case EOverlayState::HandsTied:
	case EOverlayState::Rifle:
		return LandRoll_2H;

	case EOverlayState::Injured:
	case EOverlayState::Torch:
		return LandRoll_LH;

	case EOverlayState::Pistol1H:
	case EOverlayState::Pistol2H:
		return LandRoll_RH;

	default:
		return LandRoll_Default;
	}
}

UAnimMontage* APlayerCharacter::GetGetUpAnimation(bool RagdollFaceUp) const
{
	if (RagdollFaceUp)
	{
		switch (OverlayState)
		{
		case EOverlayState::HandsTied:
			return GetUpBack_2H;

		case EOverlayState::Injured:
		case EOverlayState::Torch:
			return GetUpBack_LH;

		case EOverlayState::Pistol1H:
		case EOverlayState::Pistol2H:
		case EOverlayState::Rifle:
		case EOverlayState::Binos:
			return GetUpBack_RH;

		default:
			return GetUpBack_Default;
		}
	}
	else
	{
		switch (OverlayState)
		{
		case EOverlayState::HandsTied:
			return GetUpFront_2H;

		case EOverlayState::Injured:
		case EOverlayState::Torch:
			return GetUpFront_LH;

		case EOverlayState::Pistol1H:
		case EOverlayState::Pistol2H:
		case EOverlayState::Rifle:
		case EOverlayState::Binos:
			return GetUpFront_RH;

		default:
			return GetUpFront_Default;
		}
	}
}

void APlayerCharacter::RagdollStart()
{
	ClearHeldObject();
	Super::RagdollStart();
}

void APlayerCharacter::RagdollEnd()
{
	Super::RagdollEnd();
	UpdateHeldObject();
}
