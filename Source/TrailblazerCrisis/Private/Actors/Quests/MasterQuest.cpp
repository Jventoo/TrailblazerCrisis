// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Quests/MasterQuest.h"

// Sets default values
AMasterQuest::AMasterQuest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMasterQuest::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMasterQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

