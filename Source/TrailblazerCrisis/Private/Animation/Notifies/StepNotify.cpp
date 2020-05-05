// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/StepNotify.h"
#include "Actors/Characters/TCCharacterBase.h"
#include "TCStatics.h"
#include "Animation/HumanoidAnimInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"

#include "Engine.h"

void UStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ATCCharacterBase* Character = Cast<ATCCharacterBase>(MeshComp->GetOwner());

	if (Character)
	{
		auto FootstepsComp = Character->GetFootstepsComp();

		FootstepsComp->Play();


		FootstepsComp->SetBoolParameter(UTCStatics::FootstepsPlayParam, Character->bPlayFootsteps);


		// Possibly add a lerp and scale volume based on character speed once movement system is finalized
		FootstepsComp->SetFloatParameter(UTCStatics::FootstepsVolParam, Character->FootstepsVolume);


		// Perform a line trace at the character's feet
		FCollisionQueryParams Query(TEXT("Footsteps"), false, Character);
		Query.bReturnPhysicalMaterial = true;

		FHitResult OutHit;
		GetWorld()->LineTraceSingleByChannel(OutHit, Character->GetActorLocation(),
			Character->GetActorLocation() - FVector(0, 0, 125), ECollisionChannel::ECC_Visibility, Query);

		// Get the physical material from our hit, check if we track that surface type, and update the animinstance if we do track it
		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(MeshComp->GetAnimInstance());
		int32 index = -1;

		//auto physmat = OutHit.PhysMaterial.Get();
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, OutHit.Actor.Get()->GetFName().ToString());

		if (true || AnimInst && AnimInst->FloorTypes.Find(OutHit.PhysMaterial.Get(), index))
		{
			FootstepsComp->SetIntParameter(UTCStatics::FootstepsFloorParam, 0);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString(FString("Stepped with index") + FString::FromInt(index)));
		}
	}
}