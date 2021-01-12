// Copyright 2020 Jack Vento. All Rights Reserved.

#include "Character/Components/WeaponComponent.h"
#include "TCStatics.h"
#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"

#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxWeapons = 2;
	CurrentWeaponIdx = -1;

	// Find weapon data table
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponsObject(*(UTCStatics::WEAPON_DB_PATH));
	if (WeaponsObject.Succeeded())
	{
		WeaponsData = WeaponsObject.Object;
	}
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ATCCharacter>(GetOwner());

	if (WeaponsData != nullptr && bSpawnWeapons)
	{
		// Spawn the weapons
		SpawnWeapons();
	}
}

void UWeaponComponent::UpdateWeaponHUD()
{
}

void UWeaponComponent::SwitchWeapon(int32 WeaponIndex)
{
	if (WeaponIndex < WeaponInventory.Num())
	{
		UnequipWeapon();
		CurrentWeaponIdx = WeaponIndex;
		CurrentWeapon = WeaponInventory[WeaponIndex];
	}
}

void UWeaponComponent::SpawnWeapons()
{
	int i = 0;
	for (const auto& wep : InitialInventory)
	{
		// Search WeaponDB for specified weapon
		static const FString ContextString(TEXT("Weapon Data"));
		FWeaponData* WeaponInfo = WeaponsData->FindRow<FWeaponData>(wep.WeaponID, ContextString, true);
		if (WeaponInfo != nullptr)
		{
			// Begin spawning the weapon to fill variables used in construction script
			ABaseFirearm* SpawnedWeapon = Cast<ABaseFirearm>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
				GetWorld(), WeaponClass, FTransform(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			));
			
			// Populate member variables
			SpawnedWeapon->SetOwner(OwningCharacter);
			SpawnedWeapon->SetOwningPawn(OwningCharacter);
			SpawnedWeapon->SetStoredWeapon(wep);
			SpawnedWeapon->SetWeaponData(*WeaponInfo);

			// Finish Spawning
			UGameplayStatics::FinishSpawningActor(SpawnedWeapon, FTransform());

			// Add to inventory
			WeaponInventory.Add(SpawnedWeapon);

			// Attach to character's holster socket if the component has an unequip socket specified
			if (WeaponUnequipSockets.IsValidIndex(i)) {
				SpawnedWeapon->AttachMeshToPawn(WeaponUnequipSockets[i]);
				SpawnedWeapon->AddActorLocalRotation(WeaponInfo->DirectionFix);
			}
			OwningCharacter->SetIsArmed(true);
		}
		i++;
	}

	// Set weapon in first loadout slot as current
	SwitchWeapon(0);
}

void UWeaponComponent::SwitchFireMode()
{
	switch (CurrentWeapon->GetFireMode())
	{
	case EFireModes::Single:
		if (CurrentWeapon->GetWeaponData().BurstShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Burst);
		}
		else if (CurrentWeapon->GetWeaponData().AutoShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Auto);
		}
		break;

	case EFireModes::Burst:
		if (CurrentWeapon->GetWeaponData().AutoShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Auto);
		}
		else if (CurrentWeapon->GetWeaponData().SingleShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Single);
		}
		break;

	case EFireModes::Auto:
		if (CurrentWeapon->GetWeaponData().SingleShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Single);
		}
		else if (CurrentWeapon->GetWeaponData().BurstShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Burst);
		}
		break;
	}
}

void UWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
	if (CurrentWeapon) {
		// Find the correct socket and equip the weapon
		auto EquipSocket = WeaponEquipSockets.Find(WeaponInventory[WeaponIndex]->GetWeaponData().WeaponType);

		// Check that find succeeded
		if (EquipSocket != nullptr)
		{
			CurrentWeapon->AttachMeshToPawn(*EquipSocket);
		}

		// Update held object in character
		OwningCharacter->CurrentHeldObject = CurrentWeapon->GetWeaponMesh();

		// Move character into proper overlay state
		switch (CurrentWeapon->GetWeaponType()) {
		case EWeaponType::Rifle:
			OwningCharacter->SetOverlayState(EOverlayState::Rifle);
			break;

		case EWeaponType::Pistol:
			OwningCharacter->SetOverlayState(EOverlayState::PistolTwoHanded);
			break;

		case EWeaponType::Shotgun:
			OwningCharacter->SetOverlayState(EOverlayState::Rifle);
			break;

		case EWeaponType::Sniper:
			OwningCharacter->SetOverlayState(EOverlayState::Rifle);
			break;

		case EWeaponType::Special:
			OwningCharacter->SetOverlayState(EOverlayState::Rifle);
			break;

		default:
			OwningCharacter->SetOverlayState(EOverlayState::Rifle);
		}
	}
}

void UWeaponComponent::EquipWeapon()
{
	EquipWeapon(CurrentWeaponIdx);
}

void UWeaponComponent::UnequipWeapon(bool ReturnToHolster)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->DetachMeshFromPawn();

		// If the weapon is kept attached to the character's body on unequip, attach it
		if (ReturnToHolster) {
			// Finds unequip socket from position in inventory
			auto index = WeaponInventory.Find(CurrentWeapon);
			if (WeaponUnequipSockets.IsValidIndex(index))
			{
				CurrentWeapon->AttachMeshToPawn(WeaponUnequipSockets[index]);
			}
		}
		OwningCharacter->SetOverlayState(EOverlayState::Default);
	}
}

void UWeaponComponent::Reload()
{
	if (CanReload())
	{
		CurrentWeapon->StartReload();
	}
}

void UWeaponComponent::CycleWeapon(bool Next)
{
	if (Next)
	{
		SwitchWeapon(UKismetMathLibrary::Clamp(++CurrentWeaponIdx, 0, WeaponInventory.Num() - 1));
	}
	else
	{
		SwitchWeapon(UKismetMathLibrary::Clamp(--CurrentWeaponIdx, 0, WeaponInventory.Num() - 1));
	}
}

void UWeaponComponent::PickupWeapon(ABaseFirearm* WepRef)
{
}

void UWeaponComponent::DropWeapon()
{
}

void UWeaponComponent::AddAmmo(EAmmoType Type, int32 Amt)
{
}

void UWeaponComponent::AddRecoil(float Pitch, float Yaw)
{
	// Find multiplier (1.0-0.0) based on accuracy stat (range 0-10)
	float RecoilMultiplier = UKismetMathLibrary::MapRangeClamped(AccuracyMultiplier, 0, 10, 1.0, 0.0);

	Pitch *= RecoilMultiplier;
	Yaw *= RecoilMultiplier;

	if (OwningCharacter->GetRotationMode() != ERotationMode::Aiming)
	{
		auto Penalty = CurrentWeapon->GetWeaponData().HipAccuracyPenalty;
		Pitch *= Penalty;
		Yaw *= Penalty;
	}

	OwningCharacter->AddControllerPitchInput(Pitch);
	OwningCharacter->AddControllerYawInput(Yaw);
}

bool UWeaponComponent::CanFire() const
{
	return !IsFiring() && OwningCharacter->CanPerformAction(true) && HasWeaponEquipped();
}

bool UWeaponComponent::CanReload() const
{
	return OwningCharacter->CanPerformAction() && HasWeaponEquipped() && 
		CurrentWeapon->GetCurrentAmmoInClip() < CurrentWeapon->GetMaxAmmoPerClip();
}

bool UWeaponComponent::HasWeaponEquipped() const
{
	EOverlayState OverlayState = OwningCharacter->GetOverlayState();

	bool HasWeaponEquipped = OwningCharacter->IsArmed() && 
		(OverlayState == EOverlayState::Rifle || 
			OverlayState == EOverlayState::PistolTwoHanded || 
			OverlayState == EOverlayState::PistolOneHanded);

	return HasWeaponEquipped;
}

void UWeaponComponent::SetAiming(bool NewAimState)
{
	OwningCharacter->GetPlayerController()->ToggleCrosshair(NewAimState);
}

void UWeaponComponent::SetFiring(bool NewFireState)
{
	if (NewFireState)
	{
		CurrentWeapon->StartFire();
	}
	else
	{
		CurrentWeapon->StopFire();
	}
}

bool UWeaponComponent::IsAiming() const
{
	return OwningCharacter->GetRotationMode() == ERotationMode::Aiming;
}

bool UWeaponComponent::IsFiring() const
{
	return OwningCharacter->IsArmed() && CurrentWeapon && 
		CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}

ABaseFirearm* UWeaponComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

int32 UWeaponComponent::GetCurrentWeaponIndex() const
{
	return CurrentWeaponIdx;
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}