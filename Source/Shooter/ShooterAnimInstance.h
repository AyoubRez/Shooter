// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);


	virtual void NativeInitializeAnimation() override;

protected:
	/** Handle turning in place variables */
	void TurnInPlace();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* ShooterCharacter;

	/** The Speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Speed;
	/** The character is in the air or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;
	/** The character is moving or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsAccelerating;

	/** Offset Yaw Used For Strafing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float MovementOffsetYaw;

	/** Offset Yaw the frame before we stopped moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float LastMovementOffsetYaw;

	/** is the Character Aiming  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bAiming;

	/** Yaw of  the Character this frame   */
	float CharacterYaw;

	/** Yaw of  the Character the previous frame   */
	float CharacterYawLastFrame;

	/** Yaw of  the Character the previous frame   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn in place", meta=(AllowPrivateAccess="true"))
	float RootYawOffset;


	//Rotation Curve Value this frame 
	float RotationCurve;

	// Rotation Curve Value Last frame 
	float RotationCurveLastFrame;

	/** pitch of  the Character the previous frame   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn in place", meta=(AllowPrivateAccess="true"))
	float Pitch;
};
