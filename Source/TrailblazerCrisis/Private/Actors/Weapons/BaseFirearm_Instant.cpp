// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseFirearm_Instant.h"

#include "Player/PlayerControllerBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"



ABaseFirearm_Instant::ABaseFirearm_Instant()
{
	HitDamage = 26;
	WeaponRange = 15000;

	AllowedViewDotHitDir = -1.0f;
	MinimumProjectileSpawnDistance = 800;
	TracerRoundInterval = 3;
}


void ABaseFirearm_Instant::FireWeapon()
{
	const FVector AimDir = GetAdjustedAim();
	const FVector CameraPos = GetCameraDamageStartLocation(AimDir);
	const FVector EndPos = CameraPos + (AimDir * WeaponRange);

	/* Check for impact by tracing from the camera position */
	FHitResult Impact = WeaponTrace(CameraPos, EndPos);

	const FVector MuzzleOrigin = GetMuzzleLocation();

	FVector AdjustedAimDir = AimDir;
	if (Impact.bBlockingHit)
	{
		/* Adjust the shoot direction to hit at the crosshair. */
		AdjustedAimDir = (Impact.ImpactPoint - MuzzleOrigin).GetSafeNormal();

		/* Re-trace with the new aim direction coming out of the weapon muzzle */
		Impact = WeaponTrace(MuzzleOrigin, MuzzleOrigin + (AdjustedAimDir * WeaponRange));
	}
	else
	{
		/* Use the maximum distance as the adjust direction */
		Impact.ImpactPoint = FVector_NetQuantize(EndPos);
	}

	ProcessInstantHit(Impact, MuzzleOrigin, AdjustedAimDir);
}


void ABaseFirearm_Instant::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	float ActualHitDamage = HitDamage;

	/* Handle special damage location on the zombie body (types are setup in the Physics Asset of the zombie */
	/*USDamageType* DmgType = Cast<USDamageType>(DamageType->GetDefaultObject());
	UPhysicalMaterial* PhysMat = Impact.PhysMaterial.Get();
	if (PhysMat && DmgType)
	{
		if (PhysMat->SurfaceType == SURFACE_ZOMBIEHEAD)
		{
			ActualHitDamage *= DmgType->GetHeadDamageModifier();
		}
		else if (PhysMat->SurfaceType == SURFACE_ZOMBIELIMB)
		{
			ActualHitDamage *= DmgType->GetLimbDamageModifier();
		}
	}

	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = ActualHitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);*/
}


void ABaseFirearm_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	DealDamage(Impact, ShootDir);

	HitImpactNotify = Impact.ImpactPoint;

	SimulateInstantHit(Impact.ImpactPoint);
}


void ABaseFirearm_Instant::SimulateInstantHit(const FVector& ImpactPoint)
{
	const FVector MuzzleOrigin = GetMuzzleLocation();

	/* Adjust direction based on desired crosshair impact point and muzzle location */
	const FVector AimDir = (ImpactPoint - MuzzleOrigin).GetSafeNormal();

	const FVector EndTrace = MuzzleOrigin + (AimDir * WeaponRange);
	const FHitResult Impact = WeaponTrace(MuzzleOrigin, EndTrace);

	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTrailEffects(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffects(EndTrace);
	}
}


void ABaseFirearm_Instant::SpawnImpactEffects(const FHitResult& Impact)
{
//	if (ImpactTemplate && Impact.bBlockingHit)
//	{
//		// TODO: Possible re-trace to get hit component that is lost during replication.
//
//		/* This function prepares an actor to spawn, but requires another call to finish the actual spawn progress. This allows manipulation of properties before entering into the level */
//		ASImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<ASImpactEffect>(ImpactTemplate, FTransform(Impact.ImpactPoint.Rotation(), Impact.ImpactPoint));
//		if (EffectActor)
//		{
//			EffectActor->SurfaceHit = Impact;
//			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));
//		}
//	}
}


void ABaseFirearm_Instant::SpawnTrailEffects(const FVector& EndPoint)
{
	// Keep local count for effects
	BulletsShotCount++;

	const FVector Origin = GetMuzzleLocation();
	FVector ShootDir = EndPoint - Origin;

	// Only spawn if a minimum distance is satisfied.
	if (ShootDir.Size() < MinimumProjectileSpawnDistance)
	{
		return;
	}

	if (BulletsShotCount % TracerRoundInterval == 0)
	{
		if (TracerFX)
		{
			ShootDir.Normalize();
			UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDir.Rotation());
		}
	}
	else
	{
		// Only create trails FX by other players.
		APlayerCharacter* OwningPawn = GetPawnOwner();
		if (OwningPawn)
		{
			return;
		}

		if (TrailFX)
		{
			UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
			if (TrailPSC)
			{
				TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
			}
		}
	}
}