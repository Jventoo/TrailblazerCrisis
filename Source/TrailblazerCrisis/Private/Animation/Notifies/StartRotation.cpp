// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/StartRotation.h"
#include "Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UStartRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}