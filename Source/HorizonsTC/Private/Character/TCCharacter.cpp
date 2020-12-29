// Copyright 2020 Jack Vento. All Rights Reserved.


#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ATCCharacter::ATCCharacter()
{
}

void ATCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATCCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Binding PlayerCharInput"));

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATCCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATCCharacter::OnStopFire);

	/*PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATCCharacter::OnStartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATCCharacter::OnStopAiming);*/

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATCCharacter::OnReload);

	PlayerInputComponent->BindAction("Holster", IE_Pressed, this, &ATCCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &ATCCharacter::NextFireMode);
}


bool ATCCharacter::CanReload() const
{
	bool IsInCorrectState = MovementState == EMovementState::Grounded 
		&& MovementAction == EMovementAction::None && Gait != EGait::Sprinting;
		
	return HasWeaponEquipped() && IsInCorrectState;
}


bool ATCCharacter::CanFire() const
{
	bool IsInCorrectState = RotationMode == ERotationMode::Aiming && (MovementState == EMovementState::Grounded || MovementState == EMovementState::InAir)
		&& MovementAction == EMovementAction::None && Gait != EGait::Sprinting;

	return HasWeaponEquipped() && IsInCorrectState;
}


void ATCCharacter::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


void ATCCharacter::OnStartFire()
{
	if (CanFire() && CurrentWeapon)
	{
		if (Gait == EGait::Sprinting)
		{
			SetDesiredGait(EGait::Running);
		}

		StartWeaponFire();
	}
}


void ATCCharacter::OnStopFire()
{
	if (bIsArmed && CurrentWeapon)
	{
		StopWeaponFire();
		CurrentWeapon->DecreaseSpread();
	}
}


void ATCCharacter::StartWeaponFire()
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


void ATCCharacter::StopWeaponFire()
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

void ATCCharacter::AddRecoil(float Pitch, float Yaw)
{
	// Find multiplier (1.0-0.0) based on accuracy stat (range 0-10)
	float RecoilMultiplier = UKismetMathLibrary::MapRangeClamped(AccuracyMultiplier, 0, 10, 1.0, 0.0);

	Pitch *= RecoilMultiplier;
	Yaw *= RecoilMultiplier;

	if (RotationMode != ERotationMode::Aiming)
	{
		Pitch *= HipFirePenalty;
		Pitch *= HipFirePenalty;
	}

	AddControllerPitchInput(Pitch);
	AddControllerYawInput(Yaw);
}


void ATCCharacter::NextFireMode()
{
}

void ATCCharacter::ToggleEquip()
{
	// If we have a weapon in our inventory and the pointer to it is valid...
	if (bIsArmed && CurrentWeapon && MovementState == EMovementState::Grounded)
	{
		// ...And we are not in limited input mode, then try to equip/unequip
		if (!Cast<ATCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->IsInLimitedInputMode())
		{
			if (!CurrentWeapon->IsEquipped())
			{
				CurrentWeapon->BeginEquip();
			}
			else
			{
				CurrentWeapon->BeginUnequip();
			}
		}
	}
}

ECollisionChannel ATCCharacter::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	if (bRightShoulder)
	{
		TraceOrigin = GetMesh()->GetSocketLocation(TEXT("TP_CameraTrace_R"));
		TraceRadius = 15.0f;
	}
	else
	{
		TraceOrigin = GetMesh()->GetSocketLocation(TEXT("TP_CameraTrace_L"));
		TraceRadius = 15.0f;
	}

	return ECC_Camera;
}

FTransform ATCCharacter::GetThirdPersonPivotTarget()
{
	return FTransform(GetActorRotation(),
	                  (GetMesh()->GetSocketLocation(TEXT("Head")) + GetMesh()->GetSocketLocation(TEXT("Root"))) / 2.0f,
	                  FVector::OneVector);
}

FVector ATCCharacter::GetFirstPersonCameraTarget()
{
	return GetMesh()->GetSocketLocation(TEXT("FP_Camera"));
}
