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

public:

	/************************************************************************/
	/* Weapon Handling (MOVE MOST TO WEAPONCOMP)							*/
	/************************************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HeldObject")
		ABaseFirearm* GetCurrentWeapon() { return CurrentWeapon; }

	void AddRecoil(float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool CanReload() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = Combat)
		void NextFireMode();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void ToggleEquip();

protected:

	void OnReload();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void OnStartFire();

	UFUNCTION(BlueprintCallable, Category = Combat)
		void OnStopFire();

	void StartWeaponFire();

	virtual void StopWeaponFire() override;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName WeaponEquipSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName WeaponUnequipSocket;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
		float AccuracyMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
		float HipFirePenalty;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
		bool bIsFiring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		class ABaseFirearm* CurrentWeapon;

	bool bWantsToFire;
};
