// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseFirearm.h"
#include "Character/TCPlayerController.h"
#include "Actors/Weapons/BaseProjectile.h"

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

	BurstCounter = 0;
	AmtToBurst = 0;

	ShotsInBurst = 0;
	BulletSpeed = .0f;
	SpreadModifier = .0f;
	bCanRicochet = bBursting = bRefiring = false;
	bPlayingFireAnim = false;
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


void ABaseFirearm::BeginEquip(ATCCharacter* NewOwner)
{
	// Set this weapon's owner and start equip process
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
}


void ABaseFirearm::OnEquip(bool bPlayAnimation)
{
	bPendingEquip = true;
	DetermineWeaponState();

	// Play equip animation
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

	// Play equip sound (TODO: move to anim notify potentially)
	if (Pawn)
	{
		PlayWeaponSound(EquipSound);
	}
}


void ABaseFirearm::OnUnEquip()
{
	bIsEquipped = false;
	StopFire();

	// Stop playing any weapon animation
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

	// TODO: Cache overlay state on equip and reset back to that desired state here
	if (Pawn)
		Pawn->SetOverlayState(EOverlayState::Default);

	// TODO: Play unequip animation (equip anim in reverse?)

	// Set our weapon's current state
	DetermineWeaponState();
}


void ABaseFirearm::OnEquipFinished()
{
	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();

	if (Pawn)
	{
		// Move the gun into our hand from our holster
		Pawn->AttachToHand(nullptr, Mesh->SkeletalMesh, nullptr, false, FVector::ZeroVector);

		// TODO: Dynamically set overlay based on weapon type, assuming other types are eventually added
		Pawn->SetOverlayState(EOverlayState::Rifle);

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
	if (bBursting)
		return true;
	else
	{
		bool bPawnCanFire = Pawn && Pawn->CanFire();
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
			bBursting = true;

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
	if (MuzzleFX)
	{
		GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Cyan, FString("Muzzle fx emitted"));
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleAttachPoint, FVector(0, 0, 0), FRotator(0, 0, 0), EAttachLocation::SnapToTargetIncludingScale);
	}

	if (!bPlayingFireAnim)
	{
		GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Cyan, FString("Playing fire anim"));
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
		GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Cyan, FString("Stopping fire anim"));
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
	// Calculate projectile direction
	FTransform MainDir = CalculateMainProjectileDirection();
	FTransform FinalDir = MainDir;

	// Add spread for burst and automatic firing
	if (CurrentFireMode != EFireModes::Single)
	{
		FinalDir = CalculateFinalProjectileDirection(MainDir);
	}


	// Calculate projectile damage
	float DamageToDeal = 0;
	bool CritHit = CalculateDamage(DamageToDeal);


	// Begin spawning the projectile, initalize it, finish spawning
	ProjectileRef = GetWorld()->SpawnActorDeferred<ABaseProjectile>
		(ProjectileClass, FinalDir, Pawn, Pawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	ProjectileRef->InitializeProjectileStats(DamageToDeal, CritHit, BulletSpeed, bCanRicochet);

	UGameplayStatics::FinishSpawningActor(Cast<AActor>(ProjectileRef), FinalDir);


	// Handle recoil
	float Pitch = -1.0 * UKismetMathLibrary::RandomFloatInRange(RecoilData.UpMin, RecoilData.UpMax);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(RecoilData.RightMin, RecoilData.RightMax);

	Pawn->AddRecoil(Pitch, Yaw);
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
		AmtToBurst = UKismetMathLibrary::Min(ShotsInBurst, CurrentAmmoInClip);

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
	bBursting = false;
}


FTransform ABaseFirearm::CalculateMainProjectileDirection()
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
	}

	DrawDebugLine(GetWorld(), StartPt, EndPt, FColor::Green, false, 1, 0, 1);

	auto ARot = UKismetMathLibrary::FindLookAtRotation(MuzzlePos, HitPoint);
	auto BRot = FRotator(0.0f, 0.0f, 0.0f);//XMovepenalty, YMovepenalty, Zmovepenalty);

	auto FinalRot = UKismetMathLibrary::ComposeRotators(ARot, BRot);

	return FTransform(FinalRot, MuzzlePos, FVector(1.0, 1.0, 1.0));
}


FTransform ABaseFirearm::CalculateFinalProjectileDirection(const FTransform& MainDir)
{
	float Roll = UKismetMathLibrary::RandomFloatInRange(SpreadModifier * -1.0, SpreadModifier);
	float Pitch = UKismetMathLibrary::RandomFloatInRange(SpreadModifier * -1.0, SpreadModifier);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(SpreadModifier * -1.0, SpreadModifier);

	return FTransform(UKismetMathLibrary::ComposeRotators(MainDir.Rotator(), FRotator(Pitch, Yaw, Roll)),
		MainDir.GetLocation(), MainDir.GetScale3D());
}


bool ABaseFirearm::CalculateDamage(float& DamageOut)
{
	DamageOut = UKismetMathLibrary::RandomFloatInRange(DamageData.MinDamage, DamageData.MaxDamage);
	bool IsCrit = (DamageData.CritChance > UKismetMathLibrary::RandomFloatInRange(0.0, 1.0));

	// If we landed a crit, multiple our initial damage by our multiplier. Else, return our randomized damage
	DamageOut = IsCrit ? (DamageOut * DamageData.CritDamageMultiplier) : DamageOut;

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