// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerCharacter.h"
#include "Sound/SoundCue.h"
#include "BaseFirearm.generated.h"


UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

/**
 *
 */
UCLASS(ABSTRACT, Blueprintable)
class TRAILBLAZERCRISIS_API ABaseFirearm : public AActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float GetEquipStartedTime() const;

	float GetEquipDuration() const;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	bool bIsEquipped;

	bool bPendingEquip;

	FTimerHandle TimerHandle_HandleFiring;

	FTimerHandle EquipFinishedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float ShotsPerMinute;

protected:

	ABaseFirearm();

	/** pawn owner */
	UPROPERTY(Transient)
		class APlayerCharacter* Pawn;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh;

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	virtual void OnEquipFinished();

	bool IsEquipped() const;

	bool IsAttachedToPawn() const;

public:

	/** Map containing skeleton attach sockets in the format <Skeleton, Socket> */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firearm)
		TMap<FName, FName> AttachSockets;

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		USkeletalMeshComponent* GetWeaponMesh() const;

	virtual void OnUnEquip();

	void OnEquip(bool bPlayAnimation);

	/* Set the weapon's owning pawn */
	void SetOwningPawn(APlayerCharacter* NewOwner);

	/* Get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		class APlayerCharacter* GetPawnOwner() const;

	virtual void OnEnterInventory(APlayerCharacter* NewOwner);

	virtual void OnLeaveInventory();

	/************************************************************************/
	/* Fire & Damage Handling                                               */
	/************************************************************************/

public:

	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

	void AttachMeshToPawn();

protected:

	bool CanFire() const;

	FVector GetAdjustedAim() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	virtual void FireWeapon() PURE_VIRTUAL(ABaseFirearm::FireWeapon, );

private:

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	void OnBurstStarted();

	void OnBurstFinished();

	bool bWantsToFire;

	EWeaponState CurrentState;

	bool bRefiring;

	float LastFireTime;

	/* Time between shots for repeating fire */
	float TimeBetweenShots;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/

private:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly)
		UAnimMontage* EquipAnim;

	UPROPERTY(EditDefaultsOnly)
		UAnimMontage* FireAnim;

	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly)
		FName MuzzleAttachPoint;

	bool bPlayingFireAnim;

	UPROPERTY(Transient)
		int32 BurstCounter;

protected:

	virtual void SimulateWeaponFire();

	virtual void StopSimulatingWeaponFire();

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	UAudioComponent* PlayWeaponSound(USoundCue* SoundToPlay);

	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage* Animation);

	/************************************************************************/
	/* Ammo & Reloading                                                     */
	/************************************************************************/

private:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundCue* OutOfAmmoSound;

	FTimerHandle TimerHandle_ReloadWeapon;

	FTimerHandle TimerHandle_StopReload;

protected:

	/* Time to assign on reload when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float NoAnimReloadDuration;

	/* Time to assign on equip when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float NoEquipAnimDuration;

	UPROPERTY(Transient)
		bool bPendingReload;

	void UseAmmo();

	UPROPERTY(Transient)
		int32 CurrentAmmo;

	UPROPERTY(Transient)
		int32 CurrentAmmoInClip;

	/* Weapon ammo on spawn */
	UPROPERTY(EditDefaultsOnly)
		int32 StartAmmo;

	UPROPERTY(EditDefaultsOnly)
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly)
		int32 MaxAmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UAnimMontage* ReloadAnim;

	virtual void ReloadWeapon();

	/* Is weapon and character currently capable of starting a reload */
	bool CanReload();

public:

	virtual void StartReload();

	virtual void StopSimulateReload();

	/* Set a new total amount of ammo of weapon */
	void SetAmmoCount(int32 NewTotalAmount);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		int32 GetCurrentAmmoInClip() const;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		int32 GetMaxAmmoPerClip() const;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		int32 GetMaxAmmo() const;
};