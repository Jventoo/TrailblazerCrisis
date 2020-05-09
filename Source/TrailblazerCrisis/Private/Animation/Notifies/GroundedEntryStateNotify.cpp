// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/GroundedEntryStateNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/AnimationInterface.h"
#include "Animation/AnimInstance.h"

void UGroundedEntryStateNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	auto AnimInst = MeshComp->GetAnimInstance();
	auto AnimInstInterface = Cast<IAnimationInterface>(MeshComp);

	if (AnimInstInterface)
		IAnimationInterface::Execute_SetGroundedEntryState(AnimInst, GroundedState);
}