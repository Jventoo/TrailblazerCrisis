// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class HORIZONSTC_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Only to be called after SpawnActorDeferred()
	void InitializeProjectileStats(float fDamage, bool bCritHit, float fSpeed, bool bRicochet);

	UFUNCTION()
		void OnProjHit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Stats")
		float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Stats")
		bool CriticalHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Stats")
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Stats")
		bool Ricochet;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		UParticleSystem* CharacterImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		UParticleSystem* DefaultImpactEffect;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ProjMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* Projectile;
};
