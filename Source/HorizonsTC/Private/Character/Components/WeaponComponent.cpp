// Copyright 2020 Jack Vento. All Rights Reserved.

#include "Character/Components/WeaponComponent.h"
#include "TCStatics.h"
#include "Engine/DataTable.h"
#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxWeapons = 2;
	CurrentWeaponIdx = -1;
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ATCCharacter>(GetOwner());

	if (bSpawnWeapons)
	{
		// Find weapon data table
		static ConstructorHelpers::FObjectFinder<UDataTable> WeaponsObject(*(UTCStatics::WEAPON_DB_PATH));
		if (WeaponsObject.Succeeded())
		{
			WeaponsData = WeaponsObject.Object;

			// Spawn the weapons
			SpawnWeapons();
		}
	}
}

void UWeaponComponent::UpdateWeaponHUD()
{
}

void UWeaponComponent::SwitchWeapon(int32 WeaponIndex)
{
}

void UWeaponComponent::SpawnWeapons()
{
	for (const auto& wep : InitialInventory)
	{
		// Search WeaponDB for specified weapon
		static const FString ContextString(TEXT("Weapon Data"));
		FWeaponData* WeaponInfo = WeaponsData->FindRow<FWeaponData>(wep.WeaponID, ContextString, true);
		if (WeaponInfo)
		{
			// Begin spawning the weapon to fill variables used in construction script
			ABaseFirearm* SpawnedWeapon = Cast<ABaseFirearm>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
				GetWorld(), WeaponClass, FTransform(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			));
			
			// Populate member variables
			SpawnedWeapon->SetOwner(OwningCharacter);
			SpawnedWeapon->SetOwningPawn(OwningCharacter);
			SpawnedWeapon->StoredWeapon = wep;
			SpawnedWeapon->WeaponData = *WeaponInfo;

			// Finish Spawning
			UGameplayStatics::FinishSpawningActor(SpawnedWeapon, FTransform());

			WeaponInventory.Add(SpawnedWeapon);

			// Attach to character
			SpawnedWeapon->AttachMeshToPawn(OwningCharacter->WeaponUnequipSocket);
			SpawnedWeapon->AddActorLocalRotation(WeaponInfo->DirectionFix);
			OwningCharacter->SetIsArmed(true);
		}
	}

	SwitchWeapon(0);
}

void UWeaponComponent::SwitchFireMode()
{
	switch (CurrentWeapon->GetFireMode())
	{
	case EFireModes::Single:
		if (CurrentWeapon->WeaponData.BurstShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Burst);
		}
		else if (CurrentWeapon->WeaponData.AutoShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Auto);
		}
		break;

	case EFireModes::Burst:
		if (CurrentWeapon->WeaponData.AutoShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Auto);
		}
		else if (CurrentWeapon->WeaponData.SingleShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Single);
		}
		break;

	case EFireModes::Auto:
		if (CurrentWeapon->WeaponData.SingleShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Single);
		}
		else if (CurrentWeapon->WeaponData.BurstShot)
		{
			CurrentWeapon->SetFireMode(EFireModes::Burst);
		}
		break;
	}
}

void UWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
}

void UWeaponComponent::UnequipWeapon()
{
}

void UWeaponComponent::Reload()
{
}

void UWeaponComponent::Fire()
{
}

void UWeaponComponent::Aim()
{
}

void UWeaponComponent::CycleWeapon(bool Next)
{
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

void UWeaponComponent::SetAiming(bool NewAimState)
{
}

void UWeaponComponent::SetFiring(bool NewFireState)
{
}

bool UWeaponComponent::IsAiming() const
{
	return false;
}

bool UWeaponComponent::IsFiring() const
{
	return OwningCharacter->IsArmed() && CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}

ABaseFirearm* UWeaponComponent::GetCurrentWeapon() const
{
	return nullptr;
}

int32 UWeaponComponent::GetCurrentWeaponIndex() const
{
	return int32();
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}