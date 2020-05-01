// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseFirearm.h"
#include "Player/PlayerControllerBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ABaseFirearm::ABaseFirearm()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Mesh->bReceivesDecals = true;
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	bIsEquipped = false;
	CurrentState = EWeaponState::Idle;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	MuzzleAttachPoint = TEXT("Muzzle");

	ShotsPerMinute = 700;
	StartAmmo = 999;
	MaxAmmo = 999;
	MaxAmmoPerClip = 30;
	NoAnimReloadDuration = 1.5f;
	NoEquipAnimDuration = 0.5f;
}


void ABaseFirearm::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Setup configuration */
	TimeBetweenShots = 60.0f / ShotsPerMinute;
	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}


void ABaseFirearm::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DetachMeshFromPawn();
	StopSimulatingWeaponFire();
}


USkeletalMeshComponent* ABaseFirearm::GetWeaponMesh() const
{
	return Mesh;
}


class APlayerCharacter* ABaseFirearm::GetPawnOwner() const
{
	return Pawn;
}


void ABaseFirearm::SetOwningPawn(APlayerCharacter* NewOwner)
{
	if (Pawn != NewOwner)
	{
		SetInstigator(NewOwner);
		Pawn = NewOwner;
	}
}


void ABaseFirearm::AttachMeshToPawn(FName Socket)
{
	if (Pawn)
	{
		DetachMeshFromPawn();

		Mesh->AttachToComponent(Pawn->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
		Mesh->SetHiddenInGame(false);
	}
}


void ABaseFirearm::DetachMeshFromPawn()
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetHiddenInGame(true);
}


EFireModes ABaseFirearm::GetFireMode() const
{
	return CurrentFireMode;
}


void ABaseFirearm::SwitchToNextFireMode()
{
	switch (CurrentFireMode)
	{

	case EFireModes::Single:
		SetFireMode(EFireModes::Burst);
		break;

	case EFireModes::Burst:
		SetFireMode(EFireModes::Auto);
		break;

	case EFireModes::Auto:
		SetFireMode(EFireModes::Single);
		break;

	}
}


void ABaseFirearm::SetFireMode(EFireModes NewMode)
{
	CurrentFireMode = NewMode;
}


void ABaseFirearm::BeginEquip(APlayerCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
	OnEquip(true);
}


void ABaseFirearm::BeginUnequip()
{
	SetOwningPawn(nullptr);

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	DetachMeshFromPawn();
}


void ABaseFirearm::OnEquip(bool bPlayAnimation)
{
	bPendingEquip = true;
	DetermineWeaponState();

	if (bPlayAnimation)
	{
		float Duration = PlayWeaponAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			// Failsafe in case animation is missing
			Duration = NoEquipAnimDuration;
		}
		EquipStartedTime = GetWorld()->TimeSeconds;
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ABaseFirearm::OnEquipFinished, Duration, false);
	}
	else
	{
		/* Immediately finish equipping */
		OnEquipFinished();
	}

	if (Pawn)
	{
		PlayWeaponSound(EquipSound);
	}
}


void ABaseFirearm::OnUnEquip()
{
	bIsEquipped = false;
	StopFire();

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
	}

	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	DetermineWeaponState();
}


void ABaseFirearm::OnEquipFinished()
{
	AttachMeshToPawn(Pawn->WeaponEquipSocket);

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();

	if (Pawn)
	{
		// Try to reload empty clip
		if (CurrentAmmoInClip <= 0 &&
			CanReload())
		{
			StartReload();
		}
	}
}


bool ABaseFirearm::IsEquipped() const
{
	return bIsEquipped;
}


bool ABaseFirearm::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}


void ABaseFirearm::StartFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}


void ABaseFirearm::StopFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}


bool ABaseFirearm::CanFire() const
{
	bool bPawnCanFire = Pawn && Pawn->CanFire();
	bool bStateOK = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;
	return bPawnCanFire && bStateOK && !bPendingReload;
}


FVector ABaseFirearm::GetAdjustedAim() const
{
	APawn* MyInstigator = GetInstigator();

	APlayerControllerBase* const PC = MyInstigator ? Cast<APlayerControllerBase>(MyInstigator->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;

	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		FinalAim = CamRot.Vector();
	}
	else if (MyInstigator)
	{
		FinalAim = MyInstigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}


FVector ABaseFirearm::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	APlayerControllerBase* PC = Pawn ? Cast<APlayerControllerBase>(Pawn->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		FRotator DummyRot;
		PC->GetPlayerViewPoint(OutStartTrace, DummyRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * (FVector::DotProduct((GetInstigator()->GetActorLocation() - OutStartTrace), AimDir));
	}

	return OutStartTrace;
}


FHitResult ABaseFirearm::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

	return Hit;
}



void ABaseFirearm::HandleFiring()
{
	if (CurrentAmmoInClip > 0 && CanFire())
	{
		SimulateWeaponFire();

		if (Pawn)
		{
			FireWeapon();

			UseAmmo();

			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (Pawn)
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
		}

		/* Reload after firing last round */
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		/* Stop weapon fire FX, but stay in firing state */
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (Pawn)
	{
		/* Retrigger HandleFiring on a delay for automatic weapons */
		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseFirearm::HandleFiring, TimeBetweenShots, false);
		}
	}

	/* Make Noise on every shot. The data is managed by the PawnNoiseEmitterComponent created in SBaseCharacter and used by PawnSensingComponent in SZombieCharacter */
	/*if (Pawn)
	{
		Pawn->MakePawnNoise(1.0f);
	}*/

	LastFireTime = GetWorld()->GetTimeSeconds();
}


void ABaseFirearm::SimulateWeaponFire()
{
	if (MuzzleFX)
	{
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleAttachPoint);
	}

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	PlayWeaponSound(FireSound);
}


void ABaseFirearm::StopSimulatingWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}
}


FVector ABaseFirearm::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(MuzzleAttachPoint);
}


FVector ABaseFirearm::GetMuzzleDirection() const
{
	return Mesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}


UAudioComponent* ABaseFirearm::PlayWeaponSound(USoundCue* SoundToPlay)
{
	UAudioComponent* AC = nullptr;
	if (SoundToPlay && Pawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundToPlay, Pawn->GetRootComponent());
	}

	return AC;
}


EWeaponState ABaseFirearm::GetCurrentState() const
{
	return CurrentState;
}


void ABaseFirearm::FireWeapon()
{

}


void ABaseFirearm::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}


void ABaseFirearm::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&
		LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseFirearm::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}


void ABaseFirearm::OnBurstFinished()
{
	BurstCounter = 0;
	
	StopSimulatingWeaponFire();

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}


void ABaseFirearm::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload())
			{
				NewState = EWeaponState::Reloading;
			}
			else
			{
				NewState = CurrentState;
			}
		}
		else if (!bPendingReload && bWantsToFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}


float ABaseFirearm::GetEquipStartedTime() const
{
	return EquipStartedTime;
}


float ABaseFirearm::GetEquipDuration() const
{
	return EquipDuration;
}


float ABaseFirearm::PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (Pawn)
	{
		if (Animation)
		{
			Duration = Pawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
}


void ABaseFirearm::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (Pawn)
	{
		if (Animation)
		{
			Pawn->StopAnimMontage(Animation);
		}
	}
}



void ABaseFirearm::UseAmmo()
{
	CurrentAmmoInClip--;
	CurrentAmmo--;
}

void ABaseFirearm::SetAmmoCount(int32 NewTotalAmount)
{
	CurrentAmmo = FMath::Min(MaxAmmo, NewTotalAmount);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
}


int32 ABaseFirearm::GetCurrentAmmo() const
{
	return CurrentAmmo;
}


int32 ABaseFirearm::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}


int32 ABaseFirearm::GetMaxAmmoPerClip() const
{
	return MaxAmmoPerClip;
}


int32 ABaseFirearm::GetMaxAmmo() const
{
	return MaxAmmo;
}


void ABaseFirearm::StartReload()
{
	if (CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		float AnimDuration = PlayWeaponAnimation(ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = NoAnimReloadDuration;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseFirearm::StopSimulateReload, AnimDuration, false);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseFirearm::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);

		if (Pawn)
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}


void ABaseFirearm::StopSimulateReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
}


void ABaseFirearm::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(MaxAmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}
}


bool ABaseFirearm::CanReload()
{
	bool bCanReload = (!Pawn || Pawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < MaxAmmoPerClip) && ((CurrentAmmo - CurrentAmmoInClip) > 0);
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return (bCanReload && bGotAmmo && bStateOKToReload);
}