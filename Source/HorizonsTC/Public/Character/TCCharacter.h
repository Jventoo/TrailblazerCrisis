// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/TCBaseCharacter.h"
#include "TCCharacter.generated.h"

/**
 * Specialized character class, with additional features like held object etc.
 */
UCLASS(Blueprintable, BlueprintType)
class HORIZONSTC_API ATCCharacter : public ATCBaseCharacter
{
	GENERATED_BODY()

	ATCCharacter();

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Implement on BP to update held objects */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HeldObject")
		void UpdateHeldObject();

	UFUNCTION(BlueprintCallable, Category = "HeldObject")
		void ClearHeldObject();

	UFUNCTION(BlueprintCallable, Category = "HeldObject")
		void AttachToHand(UStaticMesh* NewStaticMesh, USkeletalMesh* NewSkeletalMesh,
						class UClass* NewAnimClass, bool bLeftHand, FVector Offset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HeldObject")
		ABaseFirearm* GetCurrentWeapon() { return CurrentWeapon;  }

	virtual void RagdollStart() override;

	virtual void RagdollEnd() override;

	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius) override;

	virtual FTransform GetThirdPersonPivotTarget() override;

	virtual FVector GetFirstPersonCameraTarget() override;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void OnOverlayStateChanged(EOverlayState PreviousState) override;

	virtual void MantleStart(
		float MantleHeight, const FComponentAndTransform& MantleLedgeWS, EMantleType MantleType) override;

	virtual void MantleEnd() override;

	/** Implement on BP to update animation states of held objects */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HeldObject")
		void UpdateHeldObjectAnimations();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* HeldObjectRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMesh = nullptr;

	/************************************************************************/
	/* Weapon Handling														*/
	/************************************************************************/

	void AddRecoil(float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Combat)
		bool GetIsArmed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
		bool IsFiring() const;

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

	void OnStartFire();

	void OnStopFire();

	void StartWeaponFire();

	virtual void StopWeaponFire() override; public:

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

	/** Indicates whether we have a weapon ready to be equipped (i.e. in holster) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
		bool bIsArmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<class ABaseFirearm> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		class ABaseFirearm* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bSpawnWeapon;

	bool bWantsToFire;
};
