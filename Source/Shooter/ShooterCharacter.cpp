// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
AShooterCharacter::AShooterCharacter():

	//Base Rates Turn/lookUp
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),

	//Turn/look up rate for aiming / not aiming 
	HipTurnRate(90.f),
	HipLookUpRate(90.F),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	//Mouse Look Sensitivity scale factore 

	MouseHipTurnRate(1.0f),
	MouseAimingLookUpRate(0.2f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.2f),
	// true when aiming the weapon 
	bAiming(false),

	//Camera field Of view 
	CameraDefaultFOV(0.f), //is gonna get set in begin play
	CameraZoomedFOV(35.f),
	ZoomInterpolationSpeed(20.f),
	CameraCurrentFOV(0.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a Camera Boom (Pulls In towards the character if there is a collision ) 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; //Camera Follows at this distance behind the character 
	CameraBoom->bUsePawnControlRotation = true; //Rotate the arm based on the character
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);


	//Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//Attach the Camera to the tip of the CameraBoom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FollowCamera->bUsePawnControlRotation = false; //Camera do not rotate relative to the arm

	// Dont rotate when the controller rotates 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Configure Character Movement 
	GetCharacterMovement()->bOrientRotationToMovement = false; //Character Moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //At this Rotation Rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle Interpolation when Zoom While Aiming 
	CameraInterpolationZoom(DeltaTime);

	//Set Look rates Based on aiming
	SetLookRates();

	//Calculate crossHairSpread multiplier 
	CalculateCrossHairsSpread(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
}

float AShooterCharacter::GetCrossHairSpreadMultiplier() const
{
	return CrossHairSpreadMultiplier;
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && Value != 0.0f)
	{
		// Find Forward Direction

		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && Value != 0.0f)
	{
		// Find Right Direction

		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//Calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec*sec/frame => deg/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	//Calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec*sec/frame => deg/frame
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketEndLocation, FVector& OutBeamLocation)
{
	//Get Current Size of the view Port 
	FVector2D ViewPortSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	// Get Screen Space Location of the crossHairs 
	FVector2D CrossHairLocation{ViewPortSize.X / 2.f, ViewPortSize.Y / 2};

	CrossHairLocation.Y -= 50.f;
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;

	//Get World Position and direction of crossHairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
	                                                               CrossHairLocation,
	                                                               CrossHairWorldPosition, CrossHairWorldDirection);

	if (bScreenToWorld) //wad deProjection successful?
	{
		FHitResult ScreenTraceHit;
		const FVector Start{CrossHairWorldPosition};
		const FVector End{CrossHairWorldPosition + CrossHairWorldDirection * 50000.f};

		//Set Beam end point to lineTraceSingleByChannel en point 
		OutBeamLocation = End;
		// Trace outward from crossHairs world Location 
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // was there a trace hit ?
		{
			//Beam is now trace hit location 
			OutBeamLocation = ScreenTraceHit.Location;
		}

		// Perform a second trace this time from the gun barel

		FHitResult WeaponTraceHit;

		const FVector WeaponTraceStart{MuzzleSocketEndLocation};
		const FVector WeaponTraceEnd{OutBeamLocation};
		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);

		if (WeaponTraceHit.bBlockingHit) //Object between barrel and hit point
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}
	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AShooterCharacter::CameraInterpolationZoom(float DeltaTime)
{
	// Set Current Camera FOV
	if (bAiming)
	{
		//Interpolat to Zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpolationSpeed);
	}
	else
	{
		//Interpolat to Default FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpolationSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrossHairsSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{0.f, 600.f};
	FVector2D VelocityMultiplierRange{0.f, 1.f};
	FVector Velocity{GetVelocity()};
	Velocity.Z = 0.f;

	CrossHairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());

	CrossHairSpreadMultiplier = 0.5f + CrossHairVelocityFactor;
}
