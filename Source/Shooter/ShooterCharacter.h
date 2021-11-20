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

	/** True when aiming  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	bool bAiming;

	/** Default Camera FOV value  */
	float CameraDefaultFOV;

	/** Zoomed Camera FOV value  */
	float CameraZoomedFOV;

	/** Current field of view in this frame*/
	float CameraCurrentFOV;

	/** interpolation speed when zooming when aiming   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float ZoomInterpolationSpeed;

public:
	/** Returns Camera Boom subObject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns Follow Camera subObject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns bAiming State */
	FORCEINLINE bool GetAiming() const { return bAiming; }

protected:
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

	/** Called when the FireButton Is Pressed*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketEndLocation, FVector& OutBeamLocation);

	/** Set bAiming to true or false */
	void AimingButtonPressed();

	void AimingButtonReleased();

	void CameraInterpolationZoom(float DeltaTime);

	//Set Base Turn rate and Base Look up rate based on aiming 
	void SetLookRates();
};
