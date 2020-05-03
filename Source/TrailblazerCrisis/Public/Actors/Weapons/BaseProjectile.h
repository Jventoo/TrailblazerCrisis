// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class TRAILBLAZERCRISIS_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	// Only to be called after SpawnActorDeferred()
	void InitializeProjectileStats(float fDamage, bool bCritHit, float fSpeed, bool bRicochet);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Stats")
		float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Stats")
		bool CriticalHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Stats")
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Stats")
		bool Ricochet;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
