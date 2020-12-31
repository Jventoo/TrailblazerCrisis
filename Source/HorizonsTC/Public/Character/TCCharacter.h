// Copyright 2020 Jack Vento. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/TCBaseCharacter.h"
#include "TCCharacter.generated.h"

/**
 * Specialized character class meant primarily to be used for the player and other complicated combat characters.
 */
UCLASS(Blueprintable, BlueprintType)
class HORIZONSTC_API ATCCharacter : public ATCBaseCharacter
{
	GENERATED_BODY()

	ATCCharacter();

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius) override;

	virtual FTransform GetThirdPersonPivotTarget() override;

	virtual FVector GetFirstPersonCameraTarget() override;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void AimPressedAction() override;

	virtual void AimReleasedAction() override;

public:
	UFUNCTION(BlueprintCallable, Category = Combat)
		void NextFireMode();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void ToggleEquip();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void OnReload();

protected:
	UFUNCTION(BlueprintCallable, Category = Combat)
		void OnStartFire();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void OnStopFire();

	/** Intermediary handling weapon inventory; communicates with the weapons themselves */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		class UWeaponComponent* WeaponComponent;

public:
	/** Returns Mesh subobject **/
	FORCEINLINE class UWeaponComponent* GetWeaponComp() const { return WeaponComponent; }

	/** Name of the WepComp. Use this name to prevent creation of the component (with ObjectInitializer.DoNotCreateDefaultSubobject). */
	static FName WeaponComponentName;
};
