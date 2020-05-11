// Copyright 2020 Cross Coast Games, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/TCCharacterBase.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class TRAILBLAZERCRISIS_API APlayerCharacter : public ATCCharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

private:

	/************************************************************************/
	/* Character Utility													*/
	/************************************************************************/


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
protected:

	/************************************************************************/
	/* Input																*/
	/************************************************************************/

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Tell anim inst and camera we're sprinting */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void StartSprinting();

	/** Tell anim inst and camera we're done sprinting */
	UFUNCTION(BlueprintCallable, Category = Movement)
		void StopSprinting();

	/**
	* Calculate movement/look direction from Fwd and Right axis values; called every tick.
	* @return  Float between [-180, 180] degrees
	*/
	UFUNCTION(BlueprintCallable, Category = Movement)
		float CalculateDirection(float ForwardValue, float RightValue);


	/************************************************************************/
	/* Weapon Handling														*/
	/************************************************************************/

public:
	/** Update members and change camera **/
	UFUNCTION(BlueprintCallable, Category = Combat)
		void ToggleEquip();

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

protected:

	void OnReload();

	void OnStartFire();

	void OnStopFire();

	void StartWeaponFire();

	void StopWeaponFire();

	void OnStartAiming();
	
	void OnStopAiming();

	UFUNCTION(BlueprintNativeEvent, Category = "Weapon|Camera")
		void UpdateCombatCamera(); 
	void UpdateCombatCamera_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Weapon|Camera")
		void UpdateAimingFOV();
	void UpdateAimingFOV_Implementation();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName WeaponEquipSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName WeaponUnequipSocket;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
		float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
		float AimFOVRatio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
		float AccuracyMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
		float HipFirePenalty;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
		bool bIsFiring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
		bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
		bool bIsArmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<class ABaseFirearm> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		class ABaseFirearm* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bSpawnWeapon;

	bool bWantsToFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Camera")
		float CombatArmOffset;

	/************************************************************************/
	/* Mantle Animation														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_2m_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_1m_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_1m_RH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_1m_LH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_1m_2H;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Mantle")
		FMantleAsset Mantle_1m_Box;

	/************************************************************************/
	/* Roll Animation														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Roll")
		class UAnimMontage* LandRoll_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Roll")
		class UAnimMontage* LandRoll_RH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Roll")
		class UAnimMontage* LandRoll_LH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Roll")
		class UAnimMontage* LandRoll_2H;

	/************************************************************************/
	/* Get Up Animation														*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpFront_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpFront_RH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpFront_LH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpFront_2H;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpBack_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpBack_RH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpBack_LH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Stand")
		class UAnimMontage* GetUpBack_2H;
};