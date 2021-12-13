// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


#pragma region  Enumerations

UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName="FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName="Reloading "),
	ECS_Equipping UMETA(DisplayName="Equipping "),

	ECS_MAX UMETA(DisplayName="Default Max")
};

#pragma endregion

#pragma region  Structur

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	/* Scene Component to use for it's location */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	/* Number of items interping to/at this scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

#pragma endregion

#pragma region Delegates
/** Equip Item Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

/** HighLight Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

#pragma endregion
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess="true"))
	float CameraDefaultFOV;

	/** Zoomed Camera FOV value  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess="true"))
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

	//The Item currently hit by our trace in trace for items can be null 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	AItem* TraceHitItem;

#pragma endregion

#pragma region Interpolation Variables

	/** Distance outward from the camera for the interp destination   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	float CameraInterpDistance;

	/** Distance Upward from the camera for the interp destination   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	float CameraInterpElevation;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* WeaponInterpComp;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp1;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp2;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp3;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp4;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp5;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	USceneComponent* InterpComp6;

	/** Array Of interp location struct   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickUpSoundTimer;

	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickUpSound;

	bool bShouldPlayEquipSound;

	void ResetPickUpSoundTimer();

	void ResetEquipSoundTimer();

	/** time to wait before we can play another pickup sound    */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	float PickUpSoundResetTime;

	/** time to wait before we can play another Equip sound   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	float EquipeSoundResetTime;

#pragma endregion

#pragma  region Ammo Variables

	/** Map to keep track of ammo of the different ammo types  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** Starting Amount of 9mm ammo   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Items", meta=(AllowPrivateAccess="true"))
	int32 Starting9mmAmmo;

	/** Starting Amount of  AR ammo   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Items", meta=(AllowPrivateAccess="true"))
	int32 StartingARAmmo;

#pragma endregion

#pragma  region Combat State

	/** Combat State can only fire or reload when unoccupied  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	ECombatState CombatState;


#pragma endregion

#pragma region Reload Variables

	/** Montage For Reload Animations   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UAnimMontage* ReloadMontage;

	/** Montage For Equip Animations   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UAnimMontage* EquipMontage;

	/** Transform of the clip when we first grab the clip when reloading    */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	FTransform ClipTransform;

	/** Scene Component to attach to the characters Hand during reloading     */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	USceneComponent* HandSceneComponent;

#pragma endregion

#pragma region Crouching

	/** True If crouching    */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bCrouching;

#pragma  endregion

#pragma region Aiming

	/** True If the aiming button is pressed     */
	bool bAimingButtonPressed;

#pragma  endregion

#pragma region Movement

	/** Regular movement speed    */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float BaseMovementSpeed;

	/** Crouching movement speed    */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float CrouchMovementSpeed;

	/** Ground friction while not crouching     */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float BaseGroundFriction;

	/** Ground friction while crouching     */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float CrouchingGroundFriction;

#pragma endregion

#pragma region Capsule

	/* Current Half height of the capsule */
	float CurrentCapsuleHalfHeight;

	/**  capsule Half height when not crouching     */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float StandingCapsuleHalfHeight;

	/** Crouching capsule Half height   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float CrouchingCapsuleHalfHeight;

#pragma endregion

#pragma region Inventory

	/** An Array Of AItems for our inventory   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{6};

#pragma endregion

#pragma region Delegate Variables

	/** Delegate for Sending Slot information to inventory bar when equipping   */
	UPROPERTY(BlueprintAssignable, Category=Delegates, meta=(AllowPrivateAccess="true"))
	FEquipItemDelegate EquipItemDelegate;

	/** Delegate for Sending Slot information for playing the icon animation   */
	UPROPERTY(BlueprintAssignable, Category=Delegates, meta=(AllowPrivateAccess="true"))
	FHighlightIconDelegate HighlightIconDelegate;

	/** The Index for the currently Highlighted slot   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess="true"))
	int32 HighLightedSlot;

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

	//FVector GetCameraInterpLocation();

	void GetPickUpItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE bool GetCrouching() const { return bCrouching; }

	FInterpLocation GetInterpLocation(int32 Index);

	int32 GetInterpLocationIndex();

	FORCEINLINE bool ShouldPlayPickUpSound() const { return bShouldPlayPickUpSound; }

	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

#pragma  endregion

#pragma  region Setters
	// Adds / subtracts to/from overlappedItemCount and Updates bShouldTraceForItems 
	void IncrementOverlappedItemCount(int8 Amount);

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	void StartPickUpSoundTimer();

	void StartEquipSoundTimer();

	void UnHighLightInventorySlot();

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

	void PlayFireSound();

	void SendBullet();

	void PlayGunFireMontage();

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
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	//Drop weapon and let ot fall to the ground function 
	void DropWeapon();

	//Drops currently equipped weapon and equips trace hit weapon is not null 
	void SwapWeapon(AWeapon* WeaponToSwap);

#pragma endregion

	/* Interaction button functions */
#pragma region Interaction
	void SelectButtonPressed();
	void SelectButtonReleased();
#pragma endregion

	/* Ammo Functions Region */
#pragma region Ammo Functions

	// Initialize the ammo map with ammo values 
	void InitializeAmmoMap();

	// Check to make sure our weapon has ammo 
	bool WeaponHasAmmo();

	// Checks to see if we have ammo ef the equipped Weapon 's ammo type  
	bool CarryingAmmo();

#pragma endregion

	/* Reload Button Functions */
#pragma region Reload

	void ReloadButtonPressed();

	// Handle Reload of the weapon 
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	//Called from animation Blueprint with GrabClip notify
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	//Called from animation Blueprint with ReplaceClip notify
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

#pragma endregion

	/* Crouch Movement */
#pragma region Crouch

	void CrouchButtonPressed();

	virtual void Jump() override;


#pragma endregion

	/* Capsule Functions  */
#pragma region Capsule

	// Interps Capsule Half height when crouching / standing 
	void InterpCapsuleHalfHeight(float DeltaTime);

#pragma endregion

	/* Aiming Functions */
#pragma region Aiming

	void Aim();

	void StopAiming();

#pragma endregion

	/* Ammo Functions */
#pragma region Ammo Functions

	void PickUpAmmo(class AAmmo* Ammo);

#pragma endregion

#pragma region Interpolation

	void InitializeInterpLocations();


#pragma endregion

#pragma region Swap Weapons


	void FKeyPressed();

	void OneKeyPressed();

	void TwoKeyPressed();

	void ThreeKeyPressed();

	void FourKeyPressed();

	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighLightInventorySlot();


#pragma endregion

#pragma endregion
};
