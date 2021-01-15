// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Engine/DataTable.h"

#include "TCStatics.h"
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

		AttachedSocket = UTCStatics::EMPTY_SOCKET;
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

	/** The socket the weapon is occupying while unequipped (can be NONE). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		FName AttachedSocket;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool bCanRicochet;
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	FWeaponData()
	{
		WeaponType = EWeaponType::Rifle;
		WeaponDamage = FFirearmDamageInfo();
		RecoilStats = FRecoilInfo();
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		UTexture2D* WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
		UTexture2D* CrosshairImg;

	/** Shots per minute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float RateOfFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		int32 MaxMagAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		int32 MaxReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		int32 RoundsInBurst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		bool SingleShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		bool BurstShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		bool AutoShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float WeaponSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float FiringSpreadMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float FiringSpreadIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float HipAccuracyPenalty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		FFirearmDamageInfo WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		FRecoilInfo RecoilStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		UParticleSystem* MuzzleFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		USoundCue* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		USoundCue* OutOfAmmoSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		UAnimMontage* FireAnim; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		UAnimMontage* ReloadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float FallbackReloadDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		UAnimMontage* EquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float FallbackEquipDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		UAnimMontage* UnequipAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleAttachPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
		FRotator DirectionFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
		FVector RightHandFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
		FVector LeftHandFix;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		bool IsEquipped() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		bool IsInHolster() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		EWeaponType GetWeaponType() const;

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

	/** Attaches mesh to the specified socket and detaches anything currently in that socket */
	void AttachMeshToPawn(FName Socket);

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

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void SetWeaponData(const FWeaponData& NewData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		FWeaponData GetWeaponData() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void SetStoredWeapon(const FInventoryWeapon& NewStored);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		FInventoryWeapon GetStoredWeapon() const;

protected:
	/** Adjust aim direction based on controller */
	FVector GetAdjustedAim() const;

	/** Adjust location if player controlled */
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

	/** Contains PREDETERMINED information/statistics about the weapon. */
	FWeaponData WeaponData;

	/** Vital runtime information used for spawning the weapon + saving its state. */
	FInventoryWeapon StoredWeapon;

	bool bWantsToFire;

	EWeaponState CurrentState;

	EFireModes CurrentFireMode;

	bool bRefiring;

	bool bBursting;

	int32 AmtToBurst;

	float LastFireTime;

	/* Time between shots for repeating fire */
	float TimeBetweenShots;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float CurrentFiringSpread;

	ABaseProjectile* ProjectileRef;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/

private:
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
	FTimerHandle TimerHandle_ReloadWeapon;
	FTimerHandle TimerHandle_StopReload;

protected:
	UPROPERTY(Transient)
		bool bPendingReload;

	void UseAmmo();

	UPROPERTY(Transient)
		int32 CurrentReserveAmmo;

	UPROPERTY(Transient)
		int32 CurrentAmmoInClip;

	virtual void ReloadWeapon();

	/* Is weapon and character currently capable of starting a reload */
	bool CanReload();

public:

	virtual void StartReload();

	virtual void StopSimulateReload();

	/* Set a new total amount of ammo of weapon */
	void SetAmmoCount(int32 NewTotalAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
		int32 GetCurrentReserveAmmo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
		int32 GetCurrentAmmoInClip() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
		int32 GetMaxAmmoPerClip() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
		int32 GetMaxReserveAmmo() const;
};
