// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/Notifies/StepNotify.h"
#include "Actors/Characters/TCCharacterBase.h"
#include "TCStatics.h"
#include "Animation/HumanoidAnimInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UStepNotify::UStepNotify()
{
	PitchMultiplier = VolumeMultiplier = 1.0f;
	OverrideMaskCurve = false;
	FootstepType = EFootstepType::Step;
}

void UStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ATCCharacterBase* Character = Cast<ATCCharacterBase>(MeshComp->GetOwner());
	if (Character)
	{
		UHumanoidAnimInstance* AnimInst = Cast<UHumanoidAnimInstance>(MeshComp->GetAnimInstance());
		auto FootstepsComp = Character->GetFootstepsComp();

		if (FootstepsComp)
		{
			// Calculate footstep volume
			float CharVolMulti = Character->FootstepsVolume;
			float CurveVal = AnimInst->GetCurveValue(UTCStatics::FOOTSTEPS_CURVE_NAME);  
			float FinalVolMulti = (CurveVal - 1) * VolumeMultiplier * CharVolMulti;

			// Perform a line trace at the character's feet
			FCollisionQueryParams Query(TEXT("Footsteps"), false, Character);
			Query.bReturnPhysicalMaterial = true;

			FHitResult OutHit(ForceInit);
			AnimInst->GetWorld()->LineTraceSingleByChannel(OutHit, Character->GetActorLocation(),
				Character->GetActorLocation() - FVector(0, 0, 120), ECollisionChannel::ECC_Visibility, Query);

			UPhysicalMaterial* PhysMat = OutHit.PhysMaterial.Get();

			// Start playing the footsteps component
			FootstepsComp->Play();
			
			FootstepsComp->SetPitchMultiplier(PitchMultiplier);

			FootstepsComp->SetBoolParameter(UTCStatics::FOOTSTEPS_PLAY_PARAM, Character->bPlayFootsteps);

			FootstepsComp->SetFloatParameter(UTCStatics::FOOTSTEPS_VOL_PARAM, (OverrideMaskCurve) ? CharVolMulti : FinalVolMulti);

			FootstepsComp->SetIntParameter(UTCStatics::FOOTSTEPS_TYPE_PARAM, (int32)FootstepType);

			// Get the physical material from our hit, check if we track that surface type, and update the animinstance if we do track it
			int32 idx = -1;
			if (AnimInst && AnimInst->FloorTypes.Find(PhysMat, idx))
			{
				FootstepsComp->SetIntParameter(UTCStatics::FOOTSTEPS_FLOOR_PARAM, idx);
			}
			else
			{
				FootstepsComp->SetIntParameter(UTCStatics::FOOTSTEPS_FLOOR_PARAM, UTCStatics::DEFAULT_FOOTSTEP_INDEX);
			}
		}
	}
}