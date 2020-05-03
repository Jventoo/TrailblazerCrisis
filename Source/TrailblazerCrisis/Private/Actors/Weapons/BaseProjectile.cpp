// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseProjectile.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseProjectile::InitializeProjectileStats(float fDamage, bool bCritHit, float fSpeed, bool bRicochet)
{
	Damage = fDamage;
	CriticalHit = bCritHit;
	Speed = fSpeed;
	Ricochet = bRicochet;
}