// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/States/OverlayOverride_NotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/AnimationInterface.h"
#include "Animation/AnimInstance.h"

UOverlayOverride_NotifyState::UOverlayOverride_NotifyState()
{
	OverrideState = 0;
}

void UOverlayOverride_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration)
{
	auto AnimInst = MeshComp->GetAnimInstance();

	if (AnimInst
		&& AnimInst->GetClass()->ImplementsInterface(UAnimationInterface::StaticClass()))
	{
		IAnimationInterface::Execute_SetOverlayOverrideState(AnimInst, OverrideState);
	}
}

void UOverlayOverride_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)
{
	auto AnimInst = MeshComp->GetAnimInstance();

	if (AnimInst
		&& AnimInst->GetClass()->ImplementsInterface(UAnimationInterface::StaticClass()))
	{
		IAnimationInterface::Execute_SetOverlayOverrideState(AnimInst, 0);
	}
}