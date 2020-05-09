// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/CameraShakeNotify.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

UCameraShakeNotify::UCameraShakeNotify()
{
	Scale = 1.0f;
}

void UCameraShakeNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	auto Pawn = Cast<APawn>(MeshComp->GetOwner());

	if (Pawn)
	{
		auto PC = Cast<APlayerController>(Pawn->GetController());

		if (PC && ShakeClass)
			PC->ClientPlayCameraShake(ShakeClass, Scale);
	}
}