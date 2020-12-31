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
	//!Cast<ATCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->IsInLimitedInputMode()
}

void UWeaponComponent::EquipWeapon()
{
	EquipWeapon(CurrentWeaponIdx);
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

void UWeaponComponent::AddRecoil(float Pitch, float Yaw)
{
	// Find multiplier (1.0-0.0) based on accuracy stat (range 0-10)
	float RecoilMultiplier = UKismetMathLibrary::MapRangeClamped(AccuracyMultiplier, 0, 10, 1.0, 0.0);

	Pitch *= RecoilMultiplier;
	Yaw *= RecoilMultiplier;

	if (OwningCharacter->GetRotationMode() != ERotationMode::Aiming)
	{
		Pitch *= CurrentWeapon->HipFirePenalty;
		Pitch *= CurrentWeapon->HipFirePenalty;
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
	return OwningCharacter->IsArmed() && CurrentWeapon && 
		CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}

ABaseFirearm* UWeaponComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

int32 UWeaponComponent::GetCurrentWeaponIndex() const
{
	return 0;
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}