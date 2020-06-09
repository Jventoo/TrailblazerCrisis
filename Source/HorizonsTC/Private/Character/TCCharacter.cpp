// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"

#include "Engine.h"

ATCCharacter::ATCCharacter()
{
	HeldObjectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectRoot"));
	HeldObjectRoot->SetupAttachment(GetMesh());

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(HeldObjectRoot);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(HeldObjectRoot);
}

void ATCCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn our weapon
	if (bSpawnWeapon && WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ABaseFirearm>(WeaponClass, SpawnParams);

		// Attach our weapon to our character's back
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwningPawn(this);
			CurrentWeapon->AttachMeshToPawn(WeaponUnequipSocket);
			
			bIsArmed = true;
		}
	}
}

void ATCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHeldObjectAnimations();
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


bool ATCCharacter::IsFiring() const
{
	return bIsArmed && CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


bool ATCCharacter::CanReload() const
{
	bool IsInCorrectState = MovementState == EMovementState::Grounded 
		&& MovementAction == EMovementAction::None && Gait != EGait::Sprinting;
		
	return HasWeaponEquipped() && IsInCorrectState;
}


bool ATCCharacter::CanFire() const
{
	bool IsInCorrectState = RotationMode == ERotationMode::Aiming && MovementState == EMovementState::Grounded
		&& MovementAction == EMovementAction::None && Gait != EGait::Sprinting;

	return HasWeaponEquipped() && IsInCorrectState;
}

void ATCCharacter::ClearHeldObject()
{
	StaticMesh->SetStaticMesh(nullptr);
	SkeletalMesh->SetSkeletalMesh(nullptr);
	SkeletalMesh->SetAnimInstanceClass(nullptr);
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
		StopWeaponFire();
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
	Pitch *= AccuracyMultiplier;
	Yaw *= AccuracyMultiplier;

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
	if (bIsArmed)
		CurrentWeapon->SwitchToNextFireMode();
}

void ATCCharacter::ToggleEquip()
{
	if (bIsArmed && CurrentWeapon)
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

void ATCCharacter::AttachToHand(UStaticMesh* NewStaticMesh, USkeletalMesh* NewSkeletalMesh, UClass* NewAnimClass, bool bLeftHand, FVector Offset)
{
	ClearHeldObject();

	/*if (IsValid(NewStaticMesh))
	{
		StaticMesh->SetStaticMesh(NewStaticMesh);
	}
	else if (IsValid(NewSkeletalMesh))
	{
		SkeletalMesh->SetSkeletalMesh(NewSkeletalMesh);
		if (IsValid(NewAnimClass))
		{
			SkeletalMesh->SetAnimInstanceClass(NewAnimClass);
		}
	}*/

	FName AttachBone;
	if (bLeftHand)
	{
		AttachBone = TEXT("VB LHS_ik_hand_gun");
	}
	else
	{
		AttachBone = TEXT("VB RHS_ik_hand_gun");
	}

	CurrentWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBone);
	CurrentWeapon->GetWeaponMesh()->SetHiddenInGame(false);
	//HeldObjectRoot->SetRelativeLocation(Offset);
}

void ATCCharacter::RagdollStart()
{
	ClearHeldObject();
	Super::RagdollStart();
}

void ATCCharacter::RagdollEnd()
{
	Super::RagdollEnd();
	UpdateHeldObject();
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

void ATCCharacter::OnOverlayStateChanged(EOverlayState PreviousState)
{
	Super::OnOverlayStateChanged(PreviousState);
	UpdateHeldObject();
}

void ATCCharacter::MantleStart(float MantleHeight, const FComponentAndTransform& MantleLedgeWS, EMantleType MantleType)
{
	Super::MantleStart(MantleHeight, MantleLedgeWS, MantleType);
	if (MantleType != EMantleType::LowMantle)
	{
		// If we're not doing low mantle, clear held object
		ClearHeldObject();
	}
}

void ATCCharacter::MantleEnd()
{
	Super::MantleEnd();
	UpdateHeldObject();
}
