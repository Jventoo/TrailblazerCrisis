// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/Animation/Notify/TCAnimNotifyGroundedEntryState.h"

#include "Character/Animation/TCCharacterAnimInstance.h"

void UTCAnimNotifyGroundedEntryState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UTCCharacterAnimInstance* AnimIns = Cast<UTCCharacterAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimIns)
	{
		AnimIns->GroundedEntryState = GroundedEntryState;
	}
}

FString UTCAnimNotifyGroundedEntryState::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Grounded Entry State: "));
	Name.Append(GetEnumerationToString(GroundedEntryState));
	return Name;
}
