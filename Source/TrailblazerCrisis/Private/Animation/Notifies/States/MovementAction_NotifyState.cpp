// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/States/MovementAction_NotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/CharacterInterface.h"

void UMovementAction_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration)
{
	auto OwningAct = MeshComp->GetOwner();

	if (OwningAct
		&& OwningAct->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		ICharacterInterface::Execute_SetMovementAction(OwningAct, MovementAction);
	}
}

void UMovementAction_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)
{
	auto OwningAct = MeshComp->GetOwner();

	if (OwningAct
		&& OwningAct->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		auto Action = ICharacterInterface::Execute_GetCurrentStates(OwningAct).MovementAction;

		if (Action == MovementAction)
			ICharacterInterface::Execute_SetMovementAction(OwningAct, EMovementAction::None);
	}
}