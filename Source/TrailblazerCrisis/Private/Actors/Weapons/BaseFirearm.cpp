// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseFirearm.h"

// Sets default values
ABaseFirearm::ABaseFirearm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseFirearm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseFirearm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

