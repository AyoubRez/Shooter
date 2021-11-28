// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "Item.h"
#include "Weapon.generated.h"


#pragma region Enumerations

UENUM(BlueprintType)
enum class EWeaponType:uint8
{
	EWT_SubmachineGun UMETA(DisplayName="SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName="AssaultRifle"),
	EWT_MAX UMETA(DisplayName="Default MAX"),
};

#pragma endregion
/**
 * 
 */
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	// Weapon Class Constructor 
	AWeapon();

	virtual void Tick(float DeltaSeconds) override;

protected:
	//Called to change Weapon State 
	void StropFalling();
private:
	// Timer for throwing weapon 
	FTimerHandle ThrowWeaponTimer;

	// Falling Time  
	float ThrowWeaponTime;

	// True if is Falling 
	bool bFalling;

	/* Ammo Count for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	int32 Ammo;


	/* Maximum Ammo That Our weapon can hold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	int32 MagazineCapacity;

	/* The Type of weapon  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	EWeaponType WeaponType;

	/* The Type of Ammo for this weapon   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	EAmmoType AmmoType;


	/* FName for the reload montage section    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	FName ReloadMontageSection;

	/** True When reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	bool bMovingClip;

	/* FName for the Clip Bone    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Properties", meta=(AllowPrivateAccess="true"))
	FName ClipBoneName;

public:
	// Called to throw Equipped weapon 
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	// Called from character class when firing weapon 
	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }

	void ReloadAmmo(int32 Amount);

	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
};
