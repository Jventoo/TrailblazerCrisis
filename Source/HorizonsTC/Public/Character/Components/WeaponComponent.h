// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Weapons/BaseFirearm.h"
#include "WeaponComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORIZONSTC_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Sets default values for this component's properties */
	UWeaponComponent();

	/** Spawn weapons and populate weapon inventory */
	void SpawnWeapons();

	/** Switch to next available fire mode */
	void SwitchFireMode();

	/**
	 * Equip specific weapon by index
	 * See overloaded function for re-equipping the default (current) weapon
	 * @param WeaponIndex - Nonnegative integer specifying weapon to equip
	 */
	void EquipWeapon(int32 WeaponIndex);

	/** 
	 * Equip the default (CurrentWeaponIdx) weapon
	 * See EquipWeapon(int32) for equipping a different weapon
	 */
	void EquipWeapon();

	/** Unequip current weapon */
	void UnequipWeapon();

	/** Attempt to reload */
	void Reload();

	/**
	 * Cycle one weapon forward or backwards in the inventory.
	 * Wrapper for SwitchWeapon().
	 * @param Next - Whether to move forward (true) or backwards (false)	
	 */
	void CycleWeapon(bool Next);

	/**
	 * Attempt to add weapon to inventory.
	 * @param WepRef - Reference to weapon object
	 */
	void PickupWeapon(ABaseFirearm* WepRef);

	/** Remove weapon from inventory */
	void DropWeapon();

	/**
	 * Add specified ammo type to inventory.
	 * @param Type - Ammo type to add
	 * @param Amt - Amount of ammo to add
	 */
	void AddAmmo(EAmmoType Type, int32 Amt);

	void AddRecoil(float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Checks")
		bool CanFire() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Checks")
		bool CanReload() const;

	/** Is the character holding a weapon in their hand? (not in holsters or inventory) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Checks")
		bool HasWeaponEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "WeaponComp|Setters")
		void SetAiming(bool NewAimState);

	UFUNCTION(BlueprintCallable, Category = "WeaponComp|Setters")
		void SetFiring(bool NewFireState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Getters")
		bool IsAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Getters")
		bool IsFiring() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Getters")
		ABaseFirearm* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Getters")
		int32 GetCurrentWeaponIndex() const;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Update HUD to reflect weapon change
	void UpdateWeaponHUD();

	/**
	 * Switch to the specified weapon specified by WeaponIndex.
	 * Not meant to be called directly. Instead, use Cycle or Equip.
	 */
	void SwitchWeapon(int32 WeaponIndex);

private:
	/**
	 * Attempt to fire
	 * Not to be called directly. Instead, use SetFiring()
	 */
	void Fire();

	/**
	 * Attempt to aim
	 * Not to be called directly. Instead, use SetAiming()
	 */
	void Aim();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		TArray<FInventoryWeapon> InitialInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		int32 MaxWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponComp)
		TArray<ABaseFirearm*> WeaponInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
		float AccuracyMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TMap<EWeaponType, FName> WeaponEquipSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TArray<FName> WeaponUnequipSocket;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		bool bSpawnWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		class UDataTable* WeaponsData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<class ABaseFirearm> WeaponClass;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		bool bShowCrosshair = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		int32 CurrentWeaponIdx;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		ABaseFirearm* CurrentWeapon;

	class ATCCharacter* OwningCharacter;
};
