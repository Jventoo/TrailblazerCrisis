// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Animation/HumanoidAnimInstance.h"
#include "TCStatics.h"

#include "Actors/Characters/TCCharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"

UHumanoidAnimInstance::UHumanoidAnimInstance()
{
	IsInAir = false;
	bReceivedInitDir = false;
	IsArmed = false;
	IsSprinting = false;
	IsAiming = false;

	bUseRootMotionValues = true;

	Speed = Direction = 0.f;
}

void UHumanoidAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = TryGetPawnOwner();
}

void UHumanoidAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
		return;

	if (Owner->IsA(ATCCharacterBase::StaticClass()))
	{
		ATCCharacterBase* Character = Cast<ATCCharacterBase>(Owner);
		if (Character)
		{
			if (bUseRootMotionValues)
			{
				IsInAir = Character->GetMovementComponent()->IsFalling();

				// Set speed to the sum of our abs inputs (max 1.0f) or our sprint speed
				auto Sum = FMath::Min(Character->GetForwardAxisValue(true)
					+ Character->GetRightAxisVal(true), UTCStatics::MAX_MOVE_SPEED);
				Speed = IsSprinting ? (Sum * UTCStatics::SPRINT_MODIFIER) : Sum;

				if (Speed > 0.01)
				{
					if (!bReceivedInitDir)
					{
						Direction = Character->GetDirection();

						if (!GetWorld()->GetTimerManager().IsTimerActive(UpdateReceivedHandle))
						{
							GetWorld()->GetTimerManager().SetTimer(UpdateReceivedHandle, this,
								&UHumanoidAnimInstance::SetReceivedDirTrue, 0.1f, false);
						}
					}
				}
				else
				{
					bReceivedInitDir = false;
				}
			}
			else
			{
				Speed = Owner->GetVelocity().Size();
				Direction = CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
			}
		}
	}
}

void UHumanoidAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();

	GetWorld()->GetTimerManager().ClearTimer(UpdateReceivedHandle);
}


void UHumanoidAnimInstance::SetReceivedDirTrue()
{
	bReceivedInitDir = true;
}