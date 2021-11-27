// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

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

public:
	// Called to throw Equipped weapon 
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	// Called from character class when firing weapon 
	void DecrementAmmo();
};
