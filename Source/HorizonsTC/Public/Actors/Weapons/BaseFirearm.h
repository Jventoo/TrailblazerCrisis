// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "BaseFirearm.generated.h"


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UENUM(BlueprintType)
enum class EFireModes : uint8
{
	Single,
	Burst,
	Auto
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol,
	Rifle,
	Shotgun,
	Sniper,
	Special
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Pistol,
	Rifle,
	Shotgun,
	Sniper,
	Rocket,
	Energy
};

USTRUCT(BlueprintType)
struct FInventoryWeapon
{
	GENERATED_BODY()

	FInventoryWeapon()
	{
		WeaponID = TEXT("None");
		CurrMagAmmo = CurrReserveAmmo = 0;
		CurrFireMode = EFireModes::Single;
		CurrWeaponState = EWeaponState::Idle;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FName WeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 CurrMagAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 CurrReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EFireModes CurrFireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponState CurrWeaponState;
};

USTRUCT(BlueprintType)
struct FRecoilInfo
{
	GENERATED_BODY()

	FRecoilInfo()
	{
		UpMin = UpMax = RightMin = RightMax = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float UpMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float UpMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float RightMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float RightMax;
};

USTRUCT(BlueprintType)
struct FFirearmDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float HeadshotDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float ProjSpeed;
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	FWeaponData()
	{
		WeaponType = EWeaponType::Rifle;
		WeaponDamage = FFirearmDamageInfo();
		RecoilStats = FRecoilInfo();
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UTexture2D* WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float RateOfFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MaxMagAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MaxReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 RoundsInBurst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool SingleShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool BurstShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool AutoShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float WeaponSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FFirearmDamageInfo WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<class ABaseProjectile> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UTexture2D* CrosshairImg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UParticleSystem* MuzzleFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FRotator DirectionFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FVector RightHandFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FVector LeftHandFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FName HolsterSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float FallbackReloadDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* WeaponFireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* ReloadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* EquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* UnequipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FRecoilInfo RecoilStats;
};

UCLASS()
class HORIZONSTC_API ABaseFirearm : public AActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	/************************************************************************/
	/* Equip & Attachments													*/
	/************************************************************************/

	float GetEquipStartedTime() const;

	float GetEquipDuration() const;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	bool bIsEquipped = false;

	bool bPendingEquip = false;

	bool bIsHolstered = false;

	FTimerHandle TimerHandle_HandleFiring;
	FTimerHandle TimerHandle_FireWeapon;
	FTimerHandle EquipFinishedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
		float ShotsPerMinute;

protected:

	ABaseFirearm();

	UPROPERTY(Transient)
		class ATCCharacter* Pawn;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh;

	virtual void OnEquipFinished();

public:

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		USkeletalMeshComponent* GetWeaponMesh() const;

	virtual void BeginEquip();

	virtual void BeginUnequip(bool ReturnToHolster = true);

	virtual void OnUnEquip(bool ReturnToHolster = true);

	void OnEquip(bool bPlayAnimation);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool IsEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool IsInHolster() const;

	/* Set the weapon's owning pawn */
	void SetOwningPawn(class ATCCharacter* NewOwner);

	/* Get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		class ATCCharacter* GetPawnOwner() const;

	/************************************************************************/
	/* Fire & Damage Handling                                               */
	/************************************************************************/

public:

	void StartFire();

	void StopFire();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon State")
		EWeaponState GetCurrentState() const;

	/* Used for putting the weapon in a character's holster (not their hand; that's in character class) */
	void AttachMeshToPawn(FName Socket, bool Detach = true);

	/** Detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon State")
		EFireModes GetFireMode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon State")
		bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon State")
		void SetFireMode(EFireModes NewMode);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon State")
		float GetCurrentSpread() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon State")
		float GetCurrentSpreadPercentage() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recoil")
		void DecreaseSpread();
	virtual void DecreaseSpread_Implementation();

	FWeaponData WeaponData;

	FInventoryWeapon StoredWeapon;

protected:

	FVector GetAdjustedAim() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	virtual void FireWeapon();

private:

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	void OnBurstStarted();

	void OnBurstFinished();

	FTransform CalculateMainProjectileDirection(FName& BoneName);

	FTransform CalculateFinalProjectileDirection(const FTransform& MainDir, const float Spread);

	bool CalculateDamage(const FName& BoneName, float& DamageOut);

	bool bWantsToFire;

	EWeaponState CurrentState;

	EFireModes CurrentFireMode;

	bool bRefiring;

	bool bBursting;

	int32 AmtToBurst;

	float LastFireTime;

	/* Time between shots for repeating fire */
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		int32 ShotsInBurst;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		float BulletSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		bool bCanRicochet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Accuracy", meta = (AllowPrivateAccess = "true"))
		FRecoilInfo RecoilData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Accuracy", meta = (AllowPrivateAccess = "true"))
		float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Accuracy", meta = (AllowPrivateAccess = "true"))
		float FiringSpreadMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Accuracy", meta = (AllowPrivateAccess = "true"))
		float FiringSpreadIncrement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Accuracy", meta = (AllowPrivateAccess = "true"))
		float FiringSpreadHipFirePenalty;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float CurrentFiringSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		FFirearmDamageInfo DamageData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ABaseProjectile> ProjectileClass;

	ABaseProjectile* ProjectileRef;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/

private:

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Sounds")
		USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Sounds")
		USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Particles")
		UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Animation")
		UAnimMontage* EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Animation")
		UAnimMontage* FireAnim;

	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
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

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Sounds")
		USoundCue* OutOfAmmoSound;

	FTimerHandle TimerHandle_ReloadWeapon;
	FTimerHandle TimerHandle_StopReload;

protected:

	/* Time to assign on reload when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Animation")
		float NoAnimReloadDuration;

	/* Time to assign on equip when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Animation")
		float NoEquipAnimDuration;

	UPROPERTY(Transient)
		bool bPendingReload;

	void UseAmmo();

	UPROPERTY(Transient)
		int32 CurrentAmmo;

	UPROPERTY(Transient)
		int32 CurrentAmmoInClip;

	/* Weapon ammo on spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
		int32 StartAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
		int32 MaxAmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Sounds")
		USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Animation")
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
