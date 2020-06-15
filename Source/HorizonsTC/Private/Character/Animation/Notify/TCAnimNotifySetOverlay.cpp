// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/Animation/Notify/TCAnimNotifySetOverlay.h"

#include "Character/TCCharacter.h"

void UTCAnimNotifySetOverlay::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}
	
	auto Player = Cast<ATCCharacter>(MeshComp->GetOwner());

	if (Player)
	{
		Player->SetOverlayState(NewOverlay);
	}
}

FString UTCAnimNotifySetOverlay::GetNotifyName_Implementation() const
{
	FString Name(TEXT("New Overlay: "));
	Name.Append(GetEnumerationToString(NewOverlay));
	return Name;
}