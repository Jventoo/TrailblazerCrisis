// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Character/Animation/Notify/TCAnimNotifyCameraShake.h"


void UTCAnimNotifyCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (OwnerPawn)
	{
		APlayerController* OwnerController = Cast<APlayerController>(OwnerPawn->GetController());
		if (OwnerController)
		{
			OwnerController->ClientStartCameraShake(ShakeClass, Scale);
		}
	}
}
