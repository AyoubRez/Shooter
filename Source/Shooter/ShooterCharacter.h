// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
#pragma  region private Components for ShooterCharacter

	/* Camera Components  */
#pragma  region  Camera

	/** CameraBoom positioning the Camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;

	/** BaseTurnRate in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float BaseTurnRate;

	/** BaseLookUpRate in deg/sec. Other scaling may affect final Look up  rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float BaseLookUpRate;

	/** turn rate when not Aiming  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float HipTurnRate;

	/** look up rate when not aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float HipLookUpRate;

	/** turn rate when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float AimingTurnRate;

	/** look up rate when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float AimingLookUpRate;

	/** Scale factor for mouse sensitivity Turn Rate when not aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseHipTurnRate;

	/** Scale factor for mouse sensitivity Look up Rate when not aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseHipLookUpRate;

	/** Scale factor for mouse sensitivity Turn Rate when  aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseAimingTurnRate;

	/** Scale factor for mouse sensitivity Look up Rate when  aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseAimingLookUpRate;

	/** Default Camera FOV value  */
	float CameraDefaultFOV;

	/** Zoomed Camera FOV value  */
	float CameraZoomedFOV;

	/** Current field of view in this frame*/
	float CameraCurrentFOV;

	/** interpolation speed when zooming when aiming   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float ZoomInterpolationSpeed;

	/** True when aiming  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	bool bAiming;

#pragma endregion

	/* Firing Components  */
#pragma  region Firing

	/** Randomized Fire Sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class USoundCue* FireSound;

	/** Flash Spawned at Barrel Socket*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UParticleSystem* MuzzleFlash;

	/** Montage for firing the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UAnimMontage* HipFireMontage;

	/** Particles Spawned about bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UParticleSystem* ImpactParticles;

	/** Smoke trail for bullets  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UParticleSystem* BeamParticles;

	float ShootTimeDuration;

	bool bFiringBullet;

	//Left Mouse button or right console trigger pressed 
	bool bFireButtonPressed;

	//True When we can fire false when waiting for timer 
	bool bShouldFire;

	//Rate of automatic gun Fire 
	float AutomaticFireRate;

	//Set a timer between gun shots 
	FTimerHandle AutoFireTimer;

	//True if we should trace for every frame for items 
	bool bShouldTraceForItems;

#pragma endregion

	/* CrossHairs Components  */
#pragma region CrossHairs

	/** Determines the spread of the crossHairs   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CrossHairs", meta=(AllowPrivateAccess="true"))
	float CrossHairSpreadMultiplier;

	/** Velocity component for crossHairs Spread   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CrossHairs", meta=(AllowPrivateAccess="true"))
	float CrossHairVelocityFactor;

	/** In Air component for crossHairs Spread   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CrossHairs", meta=(AllowPrivateAccess="true"))
	float CrossHairInAirFactor;

	/** Aim component for crossHairs Spread   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CrossHairs", meta=(AllowPrivateAccess="true"))
	float CrossHairAimFactor;

	/** Shooting component for crossHairs Spread   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CrossHairs", meta=(AllowPrivateAccess="true"))
	float CrossHairShootingFactor;

	FTimerHandle CrossHairShootTimer;

#pragma  endregion

	/* PickUp Widget Components  */
#pragma region PickUp Widget

	//Number of overlapped AItems 
	int8 OverlappedItemCount;

	//The AItem We hit last frame 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	class AItem* TraceHitItemLastFrame;

#pragma endregion

	/* Equipped Weapon Components  */
#pragma  region Equipped Weapon

	//Currently Equipped weapon 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	class AWeapon* EquippedWeapon;

	//Set in bp for the default weapon class 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

#pragma endregion

#pragma endregion

public:
#pragma region public Function for ShooterCharacter

#pragma  region  Getters

	/** Returns Camera Boom subObject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns Follow Camera subObject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns bAiming State */
	FORCEINLINE bool GetAiming() const { return bAiming; }

	/** Returns bAiming State */
	UFUNCTION(BlueprintCallable)
	float GetCrossHairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

#pragma  endregion

#pragma  region Setters
	// Adds / subtracts to/from overlappedItemCount and Updates bShouldTraceForItems 
	void IncrementOverlappedItemCount(int8 Amount);
#pragma  endregion

#pragma  endregion

protected:
#pragma region protected Functions for ShooterCharacter

	/* Movement Functions */
#pragma region Movement Functions

	//Called for forward / backwards Input
	void MoveForward(float Value);

	//Called for side to side input
	void MoveRight(float Value);

	/**
	 * Called via input to turn at given rate
	 * @param Rate This is normalized rate , i.e.  1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to look up  at given rate
	 * @param Rate This is normalized rate , i.e.  1.0 means 100% of desired look up  rate
	 */
	void LookUpAtRate(float Rate);

	/** Rotate based on mouse x movement */
	void Turn(float Value);

	/** Rotate based on mouse Y movement */
	void LookUp(float Value);

#pragma endregion

	/* Fire Functions */
#pragma region Fire Functions

	/** Called when the FireButton Is Pressed*/
	void FireWeapon();

	/** The Hit Location for the Beam */
	bool GetBeamEndLocation(const FVector& MuzzleSocketEndLocation, FVector& OutBeamLocation);

	/** Called when Fire Button is Pressed */
	void FireButtonPressed();

	/** Called when Fire Button is Released */
	void FireButtonReleased();

	/** The Time We started Firing  */
	void StartFireTimer();

	/** Called to Reset Fire Timer   */
	UFUNCTION()
	void AutoFireReset();

#pragma endregion

	/* Camera Functions */
#pragma region Camera Functions

	/** Set bAiming to true or false */
	void AimingButtonPressed();

	void AimingButtonReleased();

	// Smooth Zooming for the camera 
	void CameraInterpolationZoom(float DeltaTime);

	//Set Base Turn rate and Base Look up rate based on aiming 
	void SetLookRates();

#pragma endregion

	/* CrossHairs Functions */
#pragma region CrossHairs Functions

	// The Spread of the CrossHairs -top -down -right -left 
	void CalculateCrossHairsSpread(float DeltaTime);

	// Called When Firing Starts 
	void StartCrossHairBulletFire();

	// Called when Firing Finishes
	UFUNCTION()
	void FinishCrossHairBulletFire();

	/**  Line trace for items under the crossHairs */
	bool TraceUnderCrossHair(FHitResult& OutHitResult, FVector& OutHitLocation);

#pragma  endregion

	/* PickUp Widget Functions */
#pragma region PickUp Widget Functions

	//Trace for items if Overlapped items Count is greater than 0
	void TraceForItems();

#pragma endregion

	/* Equipped Weapon Functions */
#pragma  region Equipped Weapon Functions

	//Spawns a default weapon and attach  it to the mesh  
	AWeapon* SpawnDefaultWeapon();

	// Takes a weapon and attach it to the mesh 
	void EquipWeapon(AWeapon* WeaponToEquip);

#pragma endregion

#pragma endregion
};
