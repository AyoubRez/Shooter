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
public:
	// Called to throw Equipped weapon 
	void ThrowWeapon();
};
