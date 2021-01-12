// Copyright 2020 Jack Vento. All Rights Reserved.


#include "Actors/Weapons/BaseFirearm.h"
#include "Character/TCPlayerController.h"
#include "Actors/Weapons/BaseProjectile.h"
#include "Character/TCCharacter.h"
#include "Character/Components/WeaponComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine.h"

class ABaseProjectile;

ABaseFirearm::ABaseFirearm()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Mesh->bReceivesDecals = true;
	Mesh->CastShadow = true;
	Mesh->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	BurstCounter = 0;
	AmtToBurst = 0;

	CurrentFiringSpread = .0f;
	bBursting = bRefiring = false;
	bPlayingFireAnim = false;
}


void ABaseFirearm::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Mesh->SetSkeletalMesh(WeaponData.WeaponMesh);

	/* Setup configuration */
	TimeBetweenShots = WeaponData.RateOfFire / 60.0f;
	CurrentReserveAmmo = StoredWeapon.CurrReserveAmmo;
	CurrentAmmoInClip = StoredWeapon.CurrMagAmmo;
	CurrentState = StoredWeapon.CurrWeaponState;
	CurrentFireMode = StoredWeapon.CurrFireMode;
}


void ABaseFirearm::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DetachMeshFromPawn();
	StopSimulatingWeaponFire();
}


void ABaseFirearm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


USkeletalMeshComponent* ABaseFirearm::GetWeaponMesh() const
{
	return Mesh;
}

class ATCCharacter* ABaseFirearm::GetPawnOwner() const
{
	return Pawn;
}


void ABaseFirearm::SetOwningPawn(ATCCharacter* NewOwner)
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
		Mesh->AttachToComponent(Pawn->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
		Mesh->SetHiddenInGame(false);

		bIsHolstered = true;
	}
}


void ABaseFirearm::DetachMeshFromPawn()
{
	/*Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetHiddenInGame(true);*/

	Pawn->ClearHeldObject();

	bIsHolstered = false;
}


EFireModes ABaseFirearm::GetFireMode() const
{
	return CurrentFireMode;
}


void ABaseFirearm::SetFireMode(EFireModes NewMode)
{
	CurrentFireMode = NewMode;
}

float ABaseFirearm::GetCurrentSpread() const
{
	float Spread = WeaponData.WeaponSpread + CurrentFiringSpread;
	
	/*if (Pawn && Pawn->GetRotationMode() != ERotationMode::Aiming)
	{
		Spread *= FiringSpreadHipFirePenalty;
	}*/

	return Spread;
}

float ABaseFirearm::GetCurrentSpreadPercentage() const
{
	float Spread = CurrentFiringSpread;

	/*if (Pawn && Pawn->GetRotationMode() != ERotationMode::Aiming)
	{
		Spread *= FiringSpreadHipFirePenalty;
	}*/

	return Spread / WeaponData.FiringSpreadMax;
}


void ABaseFirearm::BeginEquip()
{
	// Moved to anim notify for better syncing
	/*if (bIsHolstered)
		DetachMeshFromPawn();*/

	OnEquip(true);
}


void ABaseFirearm::BeginUnequip(bool ReturnToHolster)
{
	if (IsEquipped() && Pawn)
	{
		// Stop firing and any other animations, then play unequip animation/sound
		OnUnEquip(ReturnToHolster);

		// To be moved to anim notify. Current in OnUnEquip until unequip montage created
		/*DetachMeshFromPawn();

		if (ReturnToHolster)
			AttachMeshToPawn(Pawn->GetWeaponComp()->WeaponUnequipSockets, false);*/
	}
}


void ABaseFirearm::OnEquip(bool bPlayAnimation)
{
	bPendingEquip = true;
	DetermineWeaponState();

	// Play equip animation
	if (bPlayAnimation)
	{
		float Duration = PlayWeaponAnimation(WeaponData.EquipAnim);
		if (Duration <= 0.0f)
		{
			// Failsafe in case animation is missing
			Duration = WeaponData.FallbackEquipDuration;

			DetachMeshFromPawn();
			Pawn->AttachToHand(nullptr, Mesh, nullptr, false, FVector::ZeroVector);
		}
		EquipStartedTime = GetWorld()->TimeSeconds;
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ABaseFirearm::OnEquipFinished, Duration, false);
	}
	else
	{
		// Move the gun into our hand from our holster
		DetachMeshFromPawn();
		Pawn->AttachToHand(nullptr, Mesh, nullptr, false, FVector::ZeroVector);

		/* Immediately finish equipping */
		OnEquipFinished();
	}

	// Play equip sound (TODO: move to anim notify potentially)
	if (Pawn)
	{
		PlayWeaponSound(WeaponData.EquipSound);
	}
}


void ABaseFirearm::OnUnEquip(bool ReturnToHolster)
{
	bIsEquipped = false;
	StopFire();

	// Stop playing any weapon animations
	if (bPendingEquip)
	{
		StopWeaponAnimation(WeaponData.EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
	}

	if (bPendingReload)
	{
		StopWeaponAnimation(WeaponData.ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	// TODO: Cache overlay state on equip and reset back to that desired state here
	if (Pawn)
	{
		Pawn->SetOverlayState(EOverlayState::Default);
	}

	// TODO: Play unequip animation, call weapon toggle notify
	DetachMeshFromPawn();

	if (ReturnToHolster)
	{
		//AttachMeshToPawn(WeaponData.HolsterSocket);
	}
	// End TODO

	// Set our weapon's current state
	DetermineWeaponState();
}


void ABaseFirearm::OnEquipFinished()
{
	Pawn->AttachToHand(nullptr, Mesh, nullptr, false, FVector::ZeroVector);

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();

	// Try to reload empty clip
	if (CurrentAmmoInClip <= 0 && CanReload())
	{
		StartReload();
	}
}


bool ABaseFirearm::IsEquipped() const
{
	return bIsEquipped;
}


bool ABaseFirearm::IsInHolster() const
{
	return bIsHolstered;
}

EWeaponType ABaseFirearm::GetWeaponType() const
{
	return WeaponData.WeaponType;
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
	if (bBursting)
	{
		return true;
	}
	else
	{
		bool bPawnCanFire = Pawn && Pawn->GetWeaponComp()->CanFire();
		bool bStateOK = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;

		return bPawnCanFire && bStateOK && !bPendingReload;
	}
}


FVector ABaseFirearm::GetAdjustedAim() const
{
	APawn* MyInstigator = GetInstigator();

	ATCPlayerController* const PC = MyInstigator ? Cast<ATCPlayerController>(MyInstigator->Controller) : nullptr;
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
	ATCPlayerController* PC = Pawn ? Cast<ATCPlayerController>(Pawn->Controller) : nullptr;
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


void ABaseFirearm::HandleFiring()
{
	if (CurrentAmmoInClip > 0 && CanFire())
	{
		if (CurrentFireMode == EFireModes::Burst)
		{
			bBursting = true;
		}

		SimulateWeaponFire();

		if (Pawn)
		{
			FireWeapon();

			UseAmmo();

			++BurstCounter;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (Pawn)
	{
		if (GetCurrentReserveAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(WeaponData.OutOfAmmoSound);
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
		/* Retrigger HandleFiring on a delay for automatic and burst modes */
		bRefiring = CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f && CurrentFireMode == EFireModes::Auto;

		if (CurrentFireMode == EFireModes::Burst)
		{
			bBursting = BurstCounter < AmtToBurst;
		}
		else
			bBursting = false;

		if (bRefiring || bBursting)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseFirearm::HandleFiring, TimeBetweenShots, false);
		}

		if (!bRefiring && !bBursting)
		{
			if (CurrentAmmoInClip <= 0 && CanReload())
			{
				StartReload();
			}

			DetermineWeaponState();
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
	if (WeaponData.MuzzleFX)
	{
		UGameplayStatics::SpawnEmitterAttached(WeaponData.MuzzleFX, Mesh, WeaponData.MuzzleAttachPoint, FVector::ZeroVector, 
			FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
	}

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(WeaponData.FireAnim);
		bPlayingFireAnim = true;
	}

	PlayWeaponSound(WeaponData.FireSound);
}


void ABaseFirearm::StopSimulatingWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(WeaponData.FireAnim);
		bPlayingFireAnim = false;
	}
}


FVector ABaseFirearm::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(WeaponData.MuzzleAttachPoint);
}


FVector ABaseFirearm::GetMuzzleDirection() const
{
	return Mesh->GetSocketRotation(WeaponData.MuzzleAttachPoint).Vector();
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
	// Calculate projectile direction
	FName Bone("null");
	FTransform MainDir = CalculateMainProjectileDirection(Bone);
	FTransform FinalDir = CalculateFinalProjectileDirection(MainDir, GetCurrentSpread());


	// Calculate projectile damage
	float DamageToDeal = 0;
	bool CritHit = CalculateDamage(Bone, DamageToDeal);


	// Begin spawning the projectile, initialize it, finish spawning
	ProjectileRef = GetWorld()->SpawnActorDeferred<ABaseProjectile>
		(WeaponData.ProjectileClass, FinalDir, Pawn, Pawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	ProjectileRef->InitializeProjectileStats(DamageToDeal, CritHit, WeaponData.WeaponDamage.ProjSpeed, WeaponData.WeaponDamage.bCanRicochet);

	UGameplayStatics::FinishSpawningActor(Cast<AActor>(ProjectileRef), FinalDir);


	// Handle recoil + spread
	float Pitch = -1.0 * UKismetMathLibrary::RandomFloatInRange(WeaponData.RecoilStats.UpMin, WeaponData.RecoilStats.UpMax);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(WeaponData.RecoilStats.RightMin, WeaponData.RecoilStats.RightMax);

	Pawn->GetWeaponComp()->AddRecoil(Pitch, Yaw);

	CurrentFiringSpread = FMath::Min(WeaponData.FiringSpreadMax, CurrentFiringSpread + WeaponData.FiringSpreadIncrement);
}


void ABaseFirearm::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing && !bBursting)
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
	if (CurrentFireMode == EFireModes::Burst)
	{
		AmtToBurst = UKismetMathLibrary::Min(WeaponData.RoundsInBurst, CurrentAmmoInClip);
	}

	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&
		LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, 
			&ABaseFirearm::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false
		);
	}
	else
	{
		HandleFiring();
	}
}


void ABaseFirearm::OnBurstFinished()
{
	BurstCounter = 0;

	// Stop effects
	StopSimulatingWeaponFire();

	// Clear firing state
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);

	bRefiring = false;
	bBursting = false;
}

void ABaseFirearm::DecreaseSpread_Implementation()
{
}

void ABaseFirearm::SetWeaponData(const FWeaponData& NewData)
{
	WeaponData = NewData;
}

FWeaponData ABaseFirearm::GetWeaponData() const
{
	return WeaponData;
}

void ABaseFirearm::SetStoredWeapon(const FInventoryWeapon& NewStored)
{
	StoredWeapon = NewStored;
}

FInventoryWeapon ABaseFirearm::GetStoredWeapon() const
{
	return StoredWeapon;
}


FTransform ABaseFirearm::CalculateMainProjectileDirection(FName& BoneName)
{
	auto SocketTransform = Mesh->GetSocketTransform(FName("Muzzle"));
	auto MuzzlePos = SocketTransform.GetLocation()
		+ (UKismetMathLibrary::GetForwardVector(SocketTransform.Rotator()) * 11.0);

	auto PC = Cast<ATCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	auto CameraTransform = PC->PlayerCameraManager->GetTransformComponent()->GetComponentTransform();

	auto StartPt = CameraTransform.GetLocation()
		+ (UKismetMathLibrary::GetForwardVector(CameraTransform.Rotator())
			* (CameraTransform.GetLocation() - MuzzlePos).Size());

	auto EndPt = CameraTransform.GetLocation()
		+ (UKismetMathLibrary::GetForwardVector(CameraTransform.Rotator()) * 10000.0);


	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(Pawn);

	FHitResult Hit(ForceInit);

	FVector HitPoint = EndPt;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartPt, EndPt, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		HitPoint = Hit.ImpactPoint;
		BoneName = Hit.BoneName;
	}

	DrawDebugLine(GetWorld(), StartPt, EndPt, FColor::Green, false, 1, 0, 1);

	auto ARot = UKismetMathLibrary::FindLookAtRotation(MuzzlePos, HitPoint);
	auto BRot = FRotator(0.0f, 0.0f, 0.0f);//XMovepenalty, YMovepenalty, Zmovepenalty);

	auto FinalRot = UKismetMathLibrary::ComposeRotators(ARot, BRot);

	return FTransform(FinalRot, MuzzlePos, FVector(1.0, 1.0, 1.0));
}


FTransform ABaseFirearm::CalculateFinalProjectileDirection(const FTransform& MainDir, const float Spread)
{
	float Roll = UKismetMathLibrary::RandomFloatInRange(Spread * -1.0, Spread);
	float Pitch = UKismetMathLibrary::RandomFloatInRange(Spread * -1.0, Spread);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(Spread * -1.0, Spread);

	return FTransform(UKismetMathLibrary::ComposeRotators(MainDir.Rotator(), FRotator(Pitch, Yaw, Roll)),
		MainDir.GetLocation(), MainDir.GetScale3D());
}


bool ABaseFirearm::CalculateDamage(const FName& BoneName, float& DamageOut)
{
	auto DamageData = WeaponData.WeaponDamage;
	DamageOut = UKismetMathLibrary::RandomFloatInRange(DamageData.MinDamage, DamageData.MaxDamage);
	bool IsCrit = BoneName == TEXT("head");

	// If we landed a crit, multiple our initial damage by our multiplier. Else, return our randomized damage
	DamageOut = IsCrit ? (DamageOut * DamageData.HeadshotDamageMultiplier) : DamageOut;

	return IsCrit;
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
		else if (bBursting || (!bPendingReload && bWantsToFire && CanFire()))
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
}


void ABaseFirearm::SetAmmoCount(int32 NewTotalAmount)
{
	CurrentReserveAmmo = FMath::Min(WeaponData.MaxReserveAmmo, NewTotalAmount);
	CurrentAmmoInClip = FMath::Min(WeaponData.MaxMagAmmo, CurrentReserveAmmo);
}


int32 ABaseFirearm::GetCurrentReserveAmmo() const
{
	return CurrentReserveAmmo;
}


int32 ABaseFirearm::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}


int32 ABaseFirearm::GetMaxAmmoPerClip() const
{
	return WeaponData.MaxMagAmmo;
}


int32 ABaseFirearm::GetMaxReserveAmmo() const
{
	return WeaponData.MaxReserveAmmo;
}


void ABaseFirearm::StartReload()
{
	if (CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		float AnimDuration = PlayWeaponAnimation(WeaponData.ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = WeaponData.FallbackReloadDuration;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseFirearm::StopSimulateReload, AnimDuration, false);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseFirearm::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);

		if (Pawn)
		{
			PlayWeaponSound(WeaponData.ReloadSound);
		}
	}
}


void ABaseFirearm::StopSimulateReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(WeaponData.ReloadAnim);
	}
}


void ABaseFirearm::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponData.MaxMagAmmo - CurrentAmmoInClip, CurrentReserveAmmo - CurrentAmmoInClip);

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}
}


bool ABaseFirearm::CanReload()
{
	bool bCanReload = (!Pawn || Pawn->GetWeaponComp()->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponData.MaxMagAmmo) && ((CurrentReserveAmmo - CurrentAmmoInClip) > 0);
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return (bCanReload && bGotAmmo && bStateOKToReload);
}