// Copyright 2020 Jack Vento. All Rights Reserved.


#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"
#include "Character/Components/WeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

FName ATCCharacter::WeaponComponentName(TEXT("WeaponComp"));

ATCCharacter::ATCCharacter()
{
	WeaponComponent = CreateOptionalDefaultSubobject<UWeaponComponent>(ATCCharacter::WeaponComponentName);
}

void ATCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATCCharacter::AimPressedAction()
{
	Super::AimPressedAction();

	if (WeaponComponent->HasWeaponEquipped())
	{
		WeaponComponent->SetAiming(true);
		GetPlayerController()->ToggleCrosshair(true);
	}
}

void ATCCharacter::AimReleasedAction()
{
	Super::AimReleasedAction();
	if (WeaponComponent->HasWeaponEquipped())
	{
		WeaponComponent->SetAiming(false);
	}
}

void ATCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATCCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if WITH_EDITORONLY_DATA
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Binding PlayerCharInput"));
#endif

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATCCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATCCharacter::OnStopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATCCharacter::OnReload);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &ATCCharacter::NextFireMode);
	PlayerInputComponent->BindAction("Holster", IE_Pressed, this, &ATCCharacter::ToggleEquip);
}

void ATCCharacter::OnStartFire()
{
	if (WeaponComponent->CanFire())
	{
		if (Gait == EGait::Sprinting)
		{
			SetDesiredGait(EGait::Running);
		}

		WeaponComponent->SetFiring(true);
	}
}

void ATCCharacter::OnStopFire()
{
	if (WeaponComponent->HasWeaponEquipped())
	{
		WeaponComponent->SetFiring(false);
	}
}

void ATCCharacter::OnReload()
{
	if (WeaponComponent->CanReload())
	{
		if (Gait == EGait::Sprinting)
		{
			SetDesiredGait(EGait::Running);
		}

		WeaponComponent->Reload();
	}
}

void ATCCharacter::NextFireMode()
{
	if (WeaponComponent->HasWeaponEquipped())
	{
		WeaponComponent->SwitchFireMode();
	}
}

void ATCCharacter::ToggleEquip()
{
	// If we have a weapon in our inventory and are ready to perform an action...
	if (IsArmed() && CanPerformAction())
	{
		if (WeaponComponent->HasWeaponEquipped())
		{
			WeaponComponent->UnequipWeapon();
		}
		else
		{
			WeaponComponent->EquipWeapon();
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
