// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/States/EarlyBlendOut_NotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/CharacterInterface.h"
#include "Animation/AnimInstance.h"

UEarlyBlendOut_NotifyState::UEarlyBlendOut_NotifyState()
{
	BlendOutTime = 0.0f;
	CheckMovementInput = CheckStance = CheckMovementInput = false;
}

void UEarlyBlendOut_NotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	auto AnimInst = MeshComp->GetAnimInstance();
	auto OwningAct = MeshComp->GetOwner();

	if (OwningAct 
		&& OwningAct->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		if (CheckMovementState)
		{
			auto State = ICharacterInterface::Execute_GetCurrentStates(OwningAct).MovementState;

			if (State == MovementStateEq)
			{
				AnimInst->Montage_Stop(BlendOutTime, Montage);
				return;
			}
		}

		if (CheckStance)
		{
			auto Stance = ICharacterInterface::Execute_GetCurrentStates(OwningAct).ActualStance;

			if (Stance == StanceEq)
			{
				AnimInst->Montage_Stop(BlendOutTime, Montage);
				return;
			}
		}

		if (CheckMovementInput)
		{
			if (ICharacterInterface::Execute_GetEssentialValues(OwningAct).HasMovementInput)
			{
				AnimInst->Montage_Stop(BlendOutTime, Montage);
				return;
			}
		}
	}
}