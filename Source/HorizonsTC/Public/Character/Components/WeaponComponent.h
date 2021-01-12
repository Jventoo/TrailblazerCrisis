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

	/**
	 * Switch to next available fire mode (if one exists).
	 * Checks WeaponData for applicable fire modes before switching.
	 */
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

	/**
	 * Unequip the current weapon.
	 * @param ReturnToHolster - Whether to move the weapon mesh to an unequip socket (true)
	 *							or simply make it disappear (false)
	 */
	void UnequipWeapon(bool ReturnToHolster = true);

	/** Attempt to reload */
	void Reload();

	/**
	 * Cycle one weapon forward or backwards in the inventory.
	 * Wrapper for SwitchWeapon().
	 * @param Next - Whether to move forward (true) or backwards (false) through the inventory
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

	/**
	 * Adds recoil to OwningCharacter. 
	 * @param Pitch - Amount of vertical recoil to add.
	 * @param Yaw - Amount of horizontal recoil to add.
	 */
	void AddRecoil(float Pitch, float Yaw);

	/** Checks if character is in the correct states to fire (not firing, overlays, movement, etc). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Checks")
		bool CanFire() const;

	/** Checks if character is in the correct states to reload (not reloading, overlays, movement, etc). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeaponComp|Checks")
		bool CanReload() const;

	/** Is the character holding a weapon in their hand? (i.e. not in holsters or inventory) */
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

public:
	/** Weapons the character starts with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponComp)
		TArray<FInventoryWeapon> InitialInventory;

	/** Maximum amount of weapons the character can have in their loadout. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponComp)
		int32 MaxWeapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = WeaponComp)
		TArray<ABaseFirearm*> WeaponInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponComp|Combat")
		float AccuracyMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponComp|Sockets")
		TMap<EWeaponType, FName> WeaponEquipSockets;

	/**
	 * List of all available sockets the character can holster weapons to. 
	 * Indexed by weapon's position in the inventory.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponComp|Sockets")
		TArray<FName> WeaponUnequipSockets;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponComp)
		bool bSpawnWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		class UDataTable* WeaponsData = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp)
		TSubclassOf<class ABaseFirearm> WeaponClass;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		bool bShowCrosshair = true;

	UPROPERTY(BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		int32 CurrentWeaponIdx;

	UPROPERTY(BlueprintReadOnly, Category = WeaponComp, meta = (AllowPrivateAccess = "true"))
		ABaseFirearm* CurrentWeapon;

	class ATCCharacter* OwningCharacter;
};
