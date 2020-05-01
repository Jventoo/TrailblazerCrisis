// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Actors/Weapons/BaseFirearm.h"
#include "Animation/HumanoidAnimInstance.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arms

	bIsCrouching = false;
	bIsSprinting = false;
	bIsJumping = false;
	bIsFiring = false;
	bIsAiming = false;
	bIsArmed = false;

	bSpawnWeapon = true;

	ForwardAxisValue = 0;
	RightAxisValue = 0;
	Direction = 0;
	
	CurrentWeapon = nullptr;

	PrimaryActorTick.bCanEverTick = true;
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn our weapon
	if (bSpawnWeapon && WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ABaseFirearm>(WeaponClass, SpawnParams);
	}

	// Attach our weapon to our character's back
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->AttachMeshToPawn(WeaponUnequipSocket);
	}
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update root motion variables
	Direction = CalculateDirection(ForwardAxisValue, RightAxisValue);
	ForwardAxisValue = InputComponent->GetAxisValue("MoveForward");
	RightAxisValue = InputComponent->GetAxisValue("MoveRight");
}


//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Vault", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnStopFire);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::OnStartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::OnStopAiming);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::OnReload);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Holster", IE_Pressed, this, &APlayerCharacter::ToggleEquip);
}


void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void APlayerCharacter::MoveForward(float Value)
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


void APlayerCharacter::MoveRight(float Value)
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


void APlayerCharacter::StartSprinting()
{
	StopWeaponFire();

	bIsSprinting = true;

	UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInst)
		AnimInst->IsSprinting = true;
}


void APlayerCharacter::StopSprinting()
{
	bIsSprinting = false;

	UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInst)
		AnimInst->IsSprinting = false;
}


void APlayerCharacter::ToggleCrouch()
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


float APlayerCharacter::GetDirection() const
{
	return Direction;
}

float APlayerCharacter::GetRightAxisVal(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(RightAxisValue);
	else
		return RightAxisValue;
}


float APlayerCharacter::GetForwardAxisValue(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(ForwardAxisValue);
	else
		return ForwardAxisValue;
}


bool APlayerCharacter::GetIsSprinting() const
{
	return bIsSprinting;
}


bool APlayerCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


float APlayerCharacter::CalculateDirection(float ForwardValue, float RightValue)
{
	FVector InputVector(ForwardValue, (RightValue * -1), 0);
	FRotator InputRot = InputVector.ToOrientationRotator();

	FRotator CameraRot = FollowCamera->GetComponentTransform().GetRotation().Rotator();
	FRotator CapsuleRot = GetCapsuleComponent()->GetComponentTransform().GetRotation().Rotator();

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(CameraRot, CapsuleRot);
	FRotator FinalRot = UKismetMathLibrary::NormalizedDeltaRotator(DeltaRot, InputRot);

	return FinalRot.Yaw;
}


void APlayerCharacter::SetDirection(float NewDir)
{
	Direction = NewDir;
}

void APlayerCharacter::SetRightAxisVal(float NewVal)
{
	RightAxisValue = NewVal;
}


void APlayerCharacter::SetForwardAxisValue(float NewVal)
{
	ForwardAxisValue = NewVal;
}


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
			AnimInst->IsArmed = true;

		bUseControllerRotationYaw = true;

		UpdateCombatCamera();
	}
	else
	{
		bIsArmed = false;

		if (CurrentWeapon)
			CurrentWeapon->BeginUnequip();

		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInst)
			AnimInst->IsArmed = false;

		bUseControllerRotationYaw = false;

		UpdateCombatCamera();
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
	if (GetIsSprinting())
	{
		StopSprinting();
	}

	bIsAiming = true;
	UpdateAimingFOV();
}


void APlayerCharacter::OnStopAiming()
{
	bIsAiming = false;
	UpdateAimingFOV();
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


// Unsmoothed version. Use timeline in BP for smoothed version
void APlayerCharacter::UpdateAimingFOV_Implementation()
{
	if (bIsAiming)
	{
		FollowCamera->SetFieldOfView(DefaultFOV * AimFOVRatio);
	}
	else
	{
		FollowCamera->SetFieldOfView(DefaultFOV);
	}
}