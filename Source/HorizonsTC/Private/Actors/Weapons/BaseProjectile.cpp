// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.


#include "Actors/Weapons/BaseProjectile.h"

#include "Character/TCBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup projectile collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	CollisionComp->SetSphereRadius(2.0f);
	
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	FWalkableSlopeOverride Slope(EWalkableSlopeBehavior::WalkableSlope_Unwalkable, .0f);
	CollisionComp->SetWalkableSlopeOverride(Slope);
	CollisionComp->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnProjHit);

	RootComponent = CollisionComp;

	// Setup projectile mesh
	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjMesh->SetupAttachment(CollisionComp);
	ProjMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjMesh->SetRelativeScale3D(FVector(0.14, 0.00721, 0.00721));

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	Projectile->InitialSpeed = Projectile->MaxSpeed = 33.0f;
	Projectile->bRotationFollowsVelocity = true;
	Projectile->ProjectileGravityScale = 0.0f;
	Projectile->bShouldBounce = true;
	Projectile->Velocity = FVector(33.0f, .0f, .0f);

	InitialLifeSpan = 5.0f;
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


void ABaseProjectile::OnProjHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// See if hit actor is a character or not
	auto Character = Cast<ATCBaseCharacter>(OtherActor);
	bool HitCharacter = (Character != nullptr);

	// Apply Damage
	UGameplayStatics::ApplyDamage(OtherActor, Damage, UGameplayStatics::GetPlayerController(GetWorld(), 0), 
		nullptr, UDamageType::StaticClass());

	// Spawn impact effect
	FTransform SpawnTrans(FRotator::ZeroRotator, Hit.Location, FVector::OneVector);
	UParticleSystem* ImpactEffect = nullptr;

	if (CharacterImpactEffect && DefaultImpactEffect)
	{
		ImpactEffect = HitCharacter ? CharacterImpactEffect : DefaultImpactEffect;
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, SpawnTrans);

	// Hide bullet if we do not ricochet it
	if (!Ricochet)
		ProjMesh->SetVisibility(false);

	// Add physics impulse to any non-character object
	if (Hit.Component->IsSimulatingPhysics() && !HitCharacter)
	{
		Hit.Component->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	SetLifeSpan(0.1f);
}