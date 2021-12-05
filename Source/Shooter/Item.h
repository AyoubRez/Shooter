// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// Enumeration Region 
#pragma region  Enumerations
UENUM(BlueprintType)
enum class EItemRarity:uint8
{
	EIR_Damaged UMETA(DisplayName="Damaged"),
	EIR_Common UMETA(DisplayName="Common"),
	EIR_Uncommon UMETA(DisplayName="Uncommon"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Legendary UMETA(DisplayName="Legendary"),
	EIR_Max UMETA(DisplayName="DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState:uint8
{
	EIS_PickUp UMETA(DisplayName="PickUp"),
	EIS_EquipInterping UMETA(DisplayName="EquipInterping"),
	EIS_PickedUp UMETA(DisplayName="PickedUp"),
	EIS_Equipped UMETA(DisplayName="Equipped"),
	EIS_Falling UMETA(DisplayName="Falling"),
	EIS_Max UMETA(DisplayName="DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType:uint8
{
	EIT_Ammo UMETA(DisplayName="Ammo"),
	EIT_Weapon UMETA(DisplayName="Weapon"),
	EIT_Max UMETA(DisplayName="DefaultMAX")
};
#pragma endregion

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
#pragma  region  protected Methodes
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called When Overlapping Area sphere 
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// Called When End Overlapping Area sphere 
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp,
	                        int32 OtherBodyIndex);

	/** Sets the ActiveStars array of Bool s based on rarity */
	void SetActiveStars();

	/** Set Properties of the items component based on state */
	virtual void SetItemProperties(EItemState State);

	/** Called when Item interp timer is finished  */
	void FinishInterping();

	//Handles Item Interpolation when in EquipItemInterping state 
	void ItemInterp(float DeltaTime);

	//Get InterpLocation based on the item type 
	FVector GetInterpLocation();

	void PlayPickUpSound();


	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();

	void UpdatePulse();

	void ResetPulseTimer();

	void StartPulseTimer();

#pragma  endregion

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Item Components 
#pragma region Item Components

	/** Skeletal Mesh for the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* ItemSkeletalMesh;

	/**Line Trace collides with box to show HUD widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class UBoxComponent* CollisionBox;

	/** Popup Widget for when the player looks at the item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* PickUpWidget;
	/** Enable Item tracing when overlapped*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class USphereComponent* AreaSphere;

	// The Name of the item 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	FString ItemName;

	// The Count of the item 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	int32 ItemCount;

	// Item Rarity determines num of starts  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	EItemRarity ItemRarity;

	// Item Rarity determines num of starts  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	TArray<bool> ActiveStars;

	//  State od the item   
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	EItemState ItemState;

	//  the Curve asset to use for the items Z location when interping   
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class UCurveFloat* ItemZCurve;

	//  Starting Location when interping begins  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	FVector ItemInterpStartLocation;

	//  Target interp location in front of the camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	FVector CameraTargetLocation;

	//  true when interping  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	bool bInterping;

	//Play when we start interping 
	FTimerHandle ItemInterpTimer;

	//  Duration if the curve of the timer   
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	float ZCurveTime;

	//  Pointer to the character  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* Character;

	// X and Y for the item while interping in the EquipeInterping state 
	float ItemInterpX;
	float ItemInterpY;

	//Initial Yaw offset between the camera and the interping item 
	float InterpInitialYawOffset;

	//  the Curve asset used to scale the item  when interping   
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	UCurveFloat* ItemScaleCurve;

	//  The sound played when item is pickedUp  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class USoundCue* PickUpSound;

	//  The sound played when item is Equipped 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	USoundCue* EquipSound;

	//  The Type of the Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	EItemType ItemType;

	//  Index of the interp location this item is interping to 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	int32 InterpLocIndex;

	//  The Index for the material we would like to change at runtime 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	int32 MaterialIndex;

	//  Dynamic  instance that we can change at runtime  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	//  Material instance used with dynamic material instance 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth;

	//  Curve to drive the dynamic material params  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	class UCurveVector* PulseCurve;


	FTimerHandle PulseTimer;

	//  Time for the pulse timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	float PulseCurveTime;

	//  Time for the pulse timer
	UPROPERTY(VisibleAnywhere, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	float GlowAmount;

	//  Time for the pulse timer
	UPROPERTY(VisibleAnywhere, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	float FresnelExponent;

	//  Time for the pulse timer
	UPROPERTY(VisibleAnywhere, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	float FresnelReflectFraction;

	//  Curve to drive the dynamic material params  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess="true"))
	UCurveVector* InterpPulseCurve;

	// BackGround for this item in the inventory  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	UTexture2D* IconBackGround;

	// Icon for this item in the inventory  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	UTexture2D* IconItem;

	// Icon for the ammo in the inventory  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	UTexture2D* AmmoIcon;


#pragma  endregion

public:
#pragma  region Getters

	FORCEINLINE UWidgetComponent* GetPickUpWidget() const { return PickUpWidget; }

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }

	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	FORCEINLINE EItemState GetItemState() const { return ItemState; }

	FORCEINLINE USkeletalMeshComponent* GetItemSkeletalMesh() const { return ItemSkeletalMesh; }

	FORCEINLINE USoundCue* GetPickupSound() const { return PickUpSound; }

	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	FORCEINLINE int32 GetItemCount() const { return ItemCount; }


#pragma endregion
#pragma region Setters

	void SetItemState(EItemState State);

#pragma endregion
#pragma region Interping
	/** Called from the AShooterCharacter class  */
	void StartItemCurve(AShooterCharacter* Char);

	//Called in aShooter character 
	void PlayEquipSound();
#pragma endregion
#pragma region CustomDepth

	virtual void EnableCustomDepth();

	virtual void DisableCustomDepth();

	void DisableGlowMaterial();

#pragma endregion
};
