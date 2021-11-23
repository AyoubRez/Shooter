// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//keep the weapon up right
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{0.f, GetItemSkeletalMesh()->GetComponentRotation().Yaw, 0.f};

		GetItemSkeletalMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{0.f, GetItemSkeletalMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemSkeletalMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{GetItemSkeletalMesh()->GetForwardVector()};
	const FVector MeshRight{GetItemSkeletalMesh()->GetRightVector()};

	//Direction in witch we throw the weapon 
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{30.f};
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector{0.f, 0.f, 1.f});

	ImpulseDirection *= 20000.f;

	GetItemSkeletalMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StropFalling, ThrowWeaponTime);
}

void AWeapon::StropFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_PickUp);
}
