// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "Item.h"
#include "WeaponType.h"
#include "Weapon.generated.h"


#pragma region Datatable

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairsMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;
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

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
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


	/* DataTable for Weapon properties    */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	/** Textures for the weapon crossHairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UTexture2D* CrossHairsMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UTexture2D* CrossHairsLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UTexture2D* CrossHairsRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UTexture2D* CrossHairsBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UTexture2D* CrossHairsTop;

	/** The Speed which automatic fire happens */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	float AutoFireRate;

	/** Particle System spawned at barrel socket when firing  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	UParticleSystem* MuzzleFlash;

	/** The Sound Played when the weapon is fired  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	USoundCue* FireSound;

	/** Name of the bone to hide on the weapon mesh   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DataTable", meta=(AllowPrivateAccess="true"))
	FName BoneToHide;

	/** Amount that the slide is pushed back during pistol fire   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	float SlideDisplacement;

	/** Curve for the slider displacement   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	UCurveFloat* SlideDisplacementCurve;

	/** Timer Handle for updating Slide displacement */
	FTimerHandle SlideTimer;

	/** Time for displacing the slide during pistol fire  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	float SlideDisplacementTime;

	/** True When moving the pistol slide   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	bool bMovingSlide;

	/** Max distance for the slide on the pistol   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	float MaxSlideDisplacement;

	/** Max rotation for pistol recoil    */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	float MaxRecoilRotation;

	/** Amount that the  pistol will rotate when  fire   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pistol", meta=(AllowPrivateAccess="true"))
	float RecoilRotation;

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

	FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }

	FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }

	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }

	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }

	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

	bool ClipIsFull();

	void StartSlideTimer();

protected:
	void FinishMovingSlide();

	void UpdateSlideDisplacement();
};
