// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Characters/TCCharacterBase.h"

#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATCCharacterBase::ATCCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bIsCrouching = false;
	bIsSprinting = false;
	bIsJumping = false;

	// Configure character movement
	auto charMove = GetCharacterMovement();
	charMove->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	charMove->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	charMove->JumpZVelocity = 600.f;
	charMove->AirControl = 0.2f;

	ForwardAxisValue = RightAxisValue = Direction = 0.0f;

	bPlayFootsteps = true;
	FootstepsVolume = 1.0f;

	FootstepsComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepsComp"));
	FootstepsComponent->SetupAttachment(RootComponent);
	FootstepsComponent->bAutoActivate = false;

}

// Called when the game starts or when spawned
void ATCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update root motion variables
	ForwardAxisValue = InputComponent->GetAxisValue("MoveForward");
	RightAxisValue = InputComponent->GetAxisValue("MoveRight");
}

// Called to bind functionality to input
void ATCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


APlayerControllerBase* ATCCharacterBase::GetPlayerController() const
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	return PC;
}


void ATCCharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ATCCharacterBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
	}
}


void ATCCharacterBase::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Dir, Value);
	}
}


void ATCCharacterBase::ToggleCrouch()
{
	if (!bIsCrouched)
	{
		bIsCrouched = true;
		GetCharacterMovement()->bWantsToCrouch = true;
	}
	else
	{
		bIsCrouched = false;
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}


float ATCCharacterBase::GetDirection() const
{
	return Direction;
}

float ATCCharacterBase::GetRightAxisVal(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(RightAxisValue);
	else
		return RightAxisValue;
}


float ATCCharacterBase::GetForwardAxisValue(bool AbsoluteVal) const
{
	if (AbsoluteVal)
		return UKismetMathLibrary::Abs(ForwardAxisValue);
	else
		return ForwardAxisValue;
}


bool ATCCharacterBase::GetIsSprinting() const
{
	return bIsSprinting;
}


void ATCCharacterBase::SetDirection(float NewDir)
{
	Direction = NewDir;
}

void ATCCharacterBase::SetRightAxisVal(float NewVal)
{
	RightAxisValue = NewVal;
}


void ATCCharacterBase::SetForwardAxisValue(float NewVal)
{
	ForwardAxisValue = NewVal;
}