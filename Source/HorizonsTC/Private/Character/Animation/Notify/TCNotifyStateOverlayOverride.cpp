// Copyright 2020 Jack Vento. All Rights Reserved.


#include "Character/Animation/Notify/TCNotifyStateOverlayOverride.h"

#include "Character/Animation/TCCharacterAnimInstance.h"

void UTCNotifyStateOverlayOverride::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	UTCCharacterAnimInstance* AnimInst = Cast<UTCCharacterAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->OverlayOverrideState = OverlayOverrideState;
	}
}

void UTCNotifyStateOverlayOverride::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UTCCharacterAnimInstance* AnimInst = Cast<UTCCharacterAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->OverlayOverrideState = 0;
	}
}

FString UTCNotifyStateOverlayOverride::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Overlay Override State: "));
	Name.Append(GetEnumerationToString(EOverlayState(OverlayOverrideState)));
	return Name;
}
