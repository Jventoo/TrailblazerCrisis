// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseFirearm.generated.h"

UCLASS()
class TRAILBLAZERCRISIS_API ABaseFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseFirearm();

	/** Map containing skeleton attach sockets in the format <Skeleton, Socket> */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firearm)
		TMap<FName, FName> AttachSockets;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
