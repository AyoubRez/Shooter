// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Ammo.h"
#include "BulletHitInterface.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "Explosive.h"
#include "Item.h"
#include "Shooter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


// Sets default values
AShooterCharacter::AShooterCharacter():

#pragma region Variable Initialization

	//Base Rates Turn/lookUp
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),

	//Turn/look up rate for aiming / not aiming 
	HipTurnRate(90.f),
	HipLookUpRate(90.F),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),

	//Mouse Look Sensitivity scale factor
	MouseHipTurnRate(1.0f),
	MouseAimingLookUpRate(0.6f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.6f),

	// true when aiming the weapon 
	bAiming(false),

	//Camera field Of view 
	CameraDefaultFOV(0.f), //is gonna get set in begin play
	CameraZoomedFOV(25.f),
	ZoomInterpolationSpeed(20.f),
	CameraCurrentFOV(0.f),

	// CrossHair Spread Factors
	CrossHairSpreadMultiplier(0.f),
	CrossHairVelocityFactor(0.f),
	CrossHairInAirFactor(0.f),
	CrossHairAimFactor(0.f),
	CrossHairShootingFactor(0.f),

	//Bullet Fire Timer variables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),

	//Automatic  fire variables 
	bShouldFire(true),
	bFireButtonPressed(false),

	//Item Trace Variables 
	bShouldTraceForItems(false),

	//Camera Interp locations  Variables 
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),

	//Starting ammo amounts 
	Starting9mmAmmo(85),
	StartingARAmmo(120),

	//Combat Variables
	CombatState(ECombatState::ECS_Unoccupied),

	//Movement
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),

	//capsule
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),

	//Aiming
	bAimingButtonPressed(false),

	//PickUp Sound Timer properties
	bShouldPlayEquipSound(true),
	bShouldPlayPickUpSound(true),
	PickUpSoundResetTime(0.2f),
	EquipeSoundResetTime(0.2f),

	// Icon Animation property
	HighLightedSlot(-1),

	// Attributes
	Health(400.f),
	MaxHealth(400.f)

#pragma endregion

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a Camera Boom (Pulls In towards the character if there is a collision ) 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 240.f; //Camera Follows at this distance behind the character 
	CameraBoom->bUsePawnControlRotation = true; //Rotate the arm based on the character
	CameraBoom->SocketOffset = FVector(0.f, 36.f, 80.f);


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

	//Create Hand scene component 
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	//Create interpolation components 
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpolationComponent"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());
	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent1"));
	InterpComp1->SetupAttachment(GetFollowCamera());
	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent2"));
	InterpComp2->SetupAttachment(GetFollowCamera());
	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent3"));
	InterpComp3->SetupAttachment(GetFollowCamera());
	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent4"));
	InterpComp4->SetupAttachment(GetFollowCamera());
	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent5"));
	InterpComp5->SetupAttachment(GetFollowCamera());
	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpolationComponent6"));
	InterpComp6->SetupAttachment(GetFollowCamera());
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		/** If Follow Camera Exists
			* Set CameraDefaultFOV
			* to FollowCamera FieldOfView
			* and CameraCurrentFOV
			* to CameraDefaultFOV
			*/
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	//Spawn the default weapon and equip it to the mesh 
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	//Create FInterpLocation struct for each interp location , Add to Array
	InitializeInterpLocations();
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

	//Trace for items Function
	TraceForItems();

	//Interp Capsule half height based on Crouching /Standing
	InterpCapsuleHalfHeight(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	//Movement Binding 
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	//Jump Binding 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Fire Binding
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	//Aiming Binding
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	//Select Binding
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	//Select Binding
	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	//Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	//SwapWeapons
	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKeyPressed);

	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::OneKeyPressed);

	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);

	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);

	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AShooterCharacter::FourKeyPressed);

	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AShooterCharacter::FiveKeyPressed);
}

void AShooterCharacter::ResetPickUpSoundTimer()
{
	bShouldPlayPickUpSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AShooterCharacter::GetCrossHairSpreadMultiplier() const
{
	// Getter for @param CrossHairSpreadMultiplier
	return CrossHairSpreadMultiplier;
}

/**Return Camera interp Location witch
 *is the Desired Location And it s public
 *so we can call it in the item class when we want to */
/*FVector AShooterCharacter::GetCameraInterpLocation()
{
	//Get Follow Camera world Location 
	const FVector CameraWorldLocation{FollowCamera->GetComponentLocation()};

	//Get Forward Vector 
	const FVector CameraForward{FollowCamera->GetForwardVector()};

	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}*/

void AShooterCharacter::GetPickUpItem(AItem* Item)
{
	Item->PlayEquipSound();
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else //Inventory full swap with Equipped weapon
		{
			SwapWeapon(Weapon);
		}
	}
	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickUpAmmo(Ammo);
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num()) return InterpLocations[Index];
	return FInterpLocation();
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	// If no Element is Overlapping
	if (OverlappedItemCount + Amount <= 0)
	{
		// Set OverlappedItemCount to 0 and Disable trace for Items
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1)return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickUpSoundTimer()
{
	bShouldPlayPickUpSound = false;
	GetWorldTimerManager().SetTimer(PickUpSoundTimer, this, &AShooterCharacter::ResetPickUpSoundTimer,
	                                PickUpSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer,
	                                EquipeSoundResetTime);
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && Value != 0.0f)
	{
		// Find Forward Direction

		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};

		//Add Movement following that Direction 
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

		//Add Movement Following the Direction 
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
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied)return;
	if (WeaponHasAmmo())
	{
		//Play Fire Sound 
		PlayFireSound();

		// Send Bullet 
		SendBullet();

		// Play Fire Montage
		PlayGunFireMontage();

		//Start Bullet Fire Timer for crossHairs
		StartCrossHairBulletFire();

		//Subtract 1 from the Weapons Ammo
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			//Start moving slide timer
			EquippedWeapon->StartSlideTimer();
		}
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketEndLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	//Check for CrossHair Trace Hit 
	FHitResult CrossHairHitResult;
	bool bCrossHairHit = TraceUnderCrossHair(CrossHairHitResult, OutBeamLocation);


	if (bCrossHairHit)
	{
		//Tentative beam location  - still need to trace from gun 
		OutBeamLocation = CrossHairHitResult.Location;
	}
	else //no CrossHair trace hit 
	{
		//OutBeamLocation is the end location for the line trace 
	}

	// Perform a second trace this time from the gun barrel
	const FVector WeaponTraceStart{MuzzleSocketEndLocation};
	const FVector StartToEnd{OutBeamLocation - MuzzleSocketEndLocation};
	const FVector WeaponTraceEnd{MuzzleSocketEndLocation + StartToEnd * 1.25f};
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);

	if (!OutHitResult.bBlockingHit) //Object between barrel and hit point
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Equipping)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
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
	//Calculate CrossHair in Velocity factor 
	const FVector2D WalkSpeedRange{0.f, 600.f};
	const FVector2D VelocityMultiplierRange{0.f, 1.f};
	FVector Velocity{GetVelocity()};
	Velocity.Z = 0.f;

	CrossHairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());
	//Calculate CrossHair in air factor 
	if (GetCharacterMovement()->IsFalling())
	{
		// inAir
		//Spread the cross hairs slowly while in air 
		CrossHairInAirFactor = FMath::FInterpTo(CrossHairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else //on The ground
	{
		//Shrink the cross hairs rapidly while Landing 
		CrossHairInAirFactor = FMath::FInterpTo(CrossHairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	//Calculate CrossHair aim factor 
	if (bAiming)
	{
		// Aiming
		//Shrink the cross hairs rapidly while aiming 
		CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else // not Aiming
	{
		//Spread the cross hairs rapidly not aiming 
		CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 0.f, DeltaTime, 30.f);
	}

	//True 0.05s after firing 
	if (bFiringBullet)
	{
		CrossHairShootingFactor = FMath::FInterpTo(CrossHairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrossHairShootingFactor = FMath::FInterpTo(CrossHairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrossHairSpreadMultiplier = 0.5f + CrossHairVelocityFactor + CrossHairInAirFactor - CrossHairAimFactor +
		CrossHairShootingFactor;
}

void AShooterCharacter::StartCrossHairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrossHairShootTimer, this, &AShooterCharacter::FinishCrossHairBulletFire,
	                                ShootTimeDuration);
}

void AShooterCharacter::FinishCrossHairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset,
	                                EquippedWeapon->GetAutoFireRate());
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

void AShooterCharacter::PlayFireSound()
{
	// Play Fire Sound 
	if (EquippedWeapon->GetFireSound())
	{
		// if FireSound Play that sound
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	//Send bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemSkeletalMesh()->
	                                                          GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		// If Barrel Socket Get transform
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemSkeletalMesh());

		if (EquippedWeapon->GetMuzzleFlash())
		{
			// if MuzzleFlash Spawn it at barrelSocket location with transform 
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		FHitResult BeamHitResult;
		// Get Beam end Location 
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// Does Hit Actor implement bulletHitInterface
			if (BeamHitResult.Actor.IsValid())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());

				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult);
				}

				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
				if (HitEnemy)
				{
					int32 Damage{};
					bool bHeadShot = false;
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						// HeadShot
						Damage = EquippedWeapon->GetHeadShotDamage();
						bHeadShot = true;
					}
					else
					{
						// Body Shot
						Damage = EquippedWeapon->GetDamage();
						bHeadShot = false;
					}
					UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage,
					                              GetController(), this, UDamageType::StaticClass());

					HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, bHeadShot);
				}
				AExplosive* HitExplosive = Cast<AExplosive>(BeamHitResult.Actor.Get());
				if (HitExplosive)
				{
					UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), EquippedWeapon->GetDamage(),
					                              GetController(), this, UDamageType::StaticClass());
				}
			}
			else
			{
				// Spawn Default particles 
				if (ImpactParticles)
				{
					// If Impact Particles Spawn them at beam end location 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
				}
			}


			// Spawn Beam particles along the X of the socket transform until the BeamEnd location 
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
			}
		}
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	// Play the recoil anime instance 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::TraceUnderCrossHair(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	//Get Current Size of the view Port 
	FVector2D ViewPortSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	// Get Screen Space Location of the crossHairs 
	FVector2D CrossHairLocation{ViewPortSize.X / 2.f, ViewPortSize.Y / 2};

	//CrossHairLocation.Y -= 50.f;
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;

	//Get World Position and direction of crossHairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
	                                                               CrossHairLocation,
	                                                               CrossHairWorldPosition, CrossHairWorldDirection);

	if (bScreenToWorld)
	{
		//Trace from CrossHair world location outward
		const FVector Start{CrossHairWorldPosition};
		const FVector End{Start + CrossHairWorldDirection * 50000.f};
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		//Trace under crossHairs
		FHitResult ItemHitResult;
		FVector HitLocation;
		TraceUnderCrossHair(ItemHitResult, HitLocation);
		if (ItemHitResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemHitResult.Actor);
			const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon)
			{
				if (HighLightedSlot == -1)
				{
					//Not Currently Highlighting a slot ; HighLight One
					HighLightInventorySlot();
				}
			}
			else
			{
				if (HighLightedSlot != -1)
				{
					// A slot is being highlighted , UnHighlightInventory slot
					UnHighLightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickUpWidget())
			{
				//Get hit item and set widget visibility
				TraceHitItem->GetPickUpWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();
				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// Inventory is full
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					// Inventory Has room
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}
			//we hit a AItem last frame 
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//Hitting a different item or AItem is null 
					TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			//Store a reference to hit item for next frame 
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// no longer overlapping any items
		//last item should not display widget
		TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	//Check subclass of variable 
	if (DefaultWeaponClass)
	{
		//Spawn the weapon 
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		//Get the hand socket 
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		if (EquippedWeapon == nullptr)
		{
			// -1 = no Equipped weapon yet. no need to reverse the icon animation
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if (!bSwapping)
		{
			// Weapon  already equipped 
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		// Set Equipped item to the newly spawned weapon 
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemSkeletalMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);

		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (EquippedWeapon == nullptr) return;

	// Do we have ammo of the correct Type ?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)StopAiming();
		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

void AShooterCharacter::FinishReloading()
{
	// Update the combat stat
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)Aim();
	if (EquippedWeapon == nullptr)return;
	const auto AmmoType{EquippedWeapon->GetAmmoType()};

	// Update the ammo map
	if (AmmoMap.Contains(AmmoType))
	{
		//Amount of ammo the character is carrying of the equipped weapon ammo type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		//Space Left in the magazine of the equipped weapon 
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();


		if (MagEmptySpace > CarriedAmmo)
		{
			// Reload the magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
		}
		else
		{
			//Fill the magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
		}
		// Update ammo type with the CarriedAmmo 
		AmmoMap.Add(AmmoType, CarriedAmmo);
	}
}

void AShooterCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr)return;

	//Index for the clip bone of the equipped weapon 
	int32 ClipBoneIndex{EquippedWeapon->GetItemSkeletalMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName())};

	//Store the transform of the clip 
	ClipTransform = EquippedWeapon->GetItemSkeletalMesh()->GetBoneTransform(ClipBoneIndex);

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);

	if (HandSceneComponent == nullptr) return;
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
	else
	{
		Super::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight{
		FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f)
	};

	// negative Value if crouching positive if standing 
	const float DeltaCapsuleHalfHeight{InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	const FVector MeshOffset{0.f, 0.f, -DeltaCapsuleHalfHeight};

	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PickUpAmmo(AAmmo* Ammo)
{
	//Check to see if ammo map contains Amos AmmoType 
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		//Get amount of ammo in our ammoMap for ammo's type
		int32 AmmoCount{AmmoMap[Ammo->GetAmmoType()]};

		AmmoCount += Ammo->GetItemCount();

		//Set the amount of ammo in the map for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		// check if gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}
	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{WeaponInterpComp, 0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{InterpComp1, 0};
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{InterpComp2, 0};
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{InterpComp3, 0};
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{InterpComp4, 0};
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{InterpComp5, 0};
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{InterpComp6, 0};
	InterpLocations.Add(InterpLoc6);
}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5)return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if ((CurrentItemIndex != NewItemIndex) && (NewItemIndex < Inventory.Num()) && ((CombatState ==
		ECombatState::ECS_Unoccupied) || (CombatState ==
		ECombatState::ECS_Equipping)))
	{
		if (bAiming)
		{
			StopAiming();
		}
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);

		EquipWeapon(NewWeapon);
		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);
		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}

		NewWeapon->PlayEquipSound(true);
	}
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}
	return -1; // Inventory is Full! 
}

void AShooterCharacter::HighLightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighLightedSlot = EmptySlot;
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;

	const FVector Start{GetActorLocation()};
	const FVector End{Start + FVector(0.f, 0.f, -400.f)};

	FCollisionQueryParams QueryParams;

	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void AShooterCharacter::UnHighLightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighLightedSlot, false);
	HighLightedSlot = -1;
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}
	return LowestIndex;
}
