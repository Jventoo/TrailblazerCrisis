// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/Animation/Notify/TCAnimNotifyToggleWeapon.h"

#include "Character/TCCharacter.h"
#include "Actors/Weapons/BaseFirearm.h"

void UTCAnimNotifyToggleWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	// Move the gun into our hand from our holster
	auto Player = Cast<ATCCharacter>(MeshComp->GetOwner());

	if (Player)
	{
		auto Weapon = Player->GetCurrentWeapon();

		if (bEquip)
		{
			// Remove from holster
			Weapon->DetachMeshFromPawn();

			// Attach to hand
			Weapon->AttachMeshToPawn(TEXT("RifleSocket"));
			
			//Player->AttachToHand(nullptr, Weapon->GetWeaponMesh(), nullptr, false, FVector::ZeroVector);
		}
		else
		{
			// Remove from hands
			Weapon->DetachMeshFromPawn();

			// Attach to back
			Weapon->AttachMeshToPawn(Player->WeaponUnequipSocket);
		}
	}
}

FString UTCAnimNotifyToggleWeapon::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Attach Weapon: "));
	Name.Append(bEquip ? "Hand" : "Holster");
	return Name;
}
