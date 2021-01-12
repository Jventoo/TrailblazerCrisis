// Copyright 2020 Jack Vento. All Rights Reserved.


#include "Character/Animation/Notify/TCAnimNotifyToggleWeapon.h"

#include "Character/TCCharacter.h"
#include "Character/Components/WeaponComponent.h"
#include "Actors/Weapons/BaseFirearm.h"

void UTCAnimNotifyToggleWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	// Get the character and check validity
	auto Character = Cast<ATCCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		// Check that character has a weapon component
		auto WeaponComp = Character->GetWeaponComp();
		if (WeaponComp)
		{
			// Either equip or unequip depending on the type animation the notify is in
			if (bEquip)
			{
				WeaponComp->EquipWeapon();
			}
			else
			{
				WeaponComp->UnequipWeapon();
			}
		}
	}
}

FString UTCAnimNotifyToggleWeapon::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Attach Weapon: "));
	Name.Append(bEquip ? "Hand" : "Holster");
	return Name;
}
