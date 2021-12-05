// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "Item.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()

public:
	AAmmo();

	virtual void Tick(float DeltaSeconds) override;

private:
	/** Mesh for the Ammo Pick up */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ammo", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* AmmoMesh;

	/** Type of the ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo", meta=(AllowPrivateAccess="true"))
	EAmmoType AmmoType;

	/** The texture for the ammo icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo", meta=(AllowPrivateAccess="true"))
	UTexture2D* AmmoIconTexture;

	/** Overlap Sphere for picking up the ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo", meta=(AllowPrivateAccess="true"))
	class USphereComponent* AmmoCollisionSphere;

protected:
	virtual void BeginPlay() override;

	//Override of setItemProperties so we can set AmmoMesh Properties
	virtual void SetItemProperties(EItemState State) override;


public:
#pragma  region Getters

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

#pragma endregion

#pragma region  CustomDepth

	virtual void EnableCustomDepth() override;

	virtual void DisableCustomDepth() override;

#pragma endregion
protected:
	// Called When Overlapping Area sphere 
	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
