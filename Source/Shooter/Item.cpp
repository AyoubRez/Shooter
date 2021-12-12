// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem():
#pragma region Variable Initialization
	// Item Variables 
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_PickUp),
	// Item Interp variables 
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_Max),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),

	//Dynamic material params
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	PulseCurveTime(5.f),
	SlotIndex(0),
	bCharacterInventoryFull(false)

#pragma endregion


{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create ItemSkeletalMesh and Set it as root component 
	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	SetRootComponent(ItemSkeletalMesh);


	// Create CollisionBox and Attach  it to ItemSkeletalMesh 
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemSkeletalMesh);

	// Collision Box Ignores all channels and Blocks Visibility channel 
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Create PickUpWidget and Attach it to rootComponent 
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(GetRootComponent());

	// Create AreaSphere and Attach it to RootComponent 
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
	//Set Sphere Radius to 160.f
	AreaSphere->SetSphereRadius(160.f);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// On Begin Play check for PickUpWidget And Hide it 
	if (PickUpWidget)
	{
		//Hide PickUp Widget
		PickUpWidget->SetVisibility(false);
	}

	//Set active stars array based on item rarity
	SetActiveStars();


	//Setup Overlap for Area Sphere 
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//Set Item properties based on item State
	SetItemProperties(ItemState);

	//Set Custom Depth to disabled 
	InitializeCustomDepth();

	StartPulseTimer();
}

// Function to trigger on Begin overlap with AreaSphere
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If Other Actor is detected 
	if (OtherActor)
	{
		// try cast Other Actor to ShooterCharacter to check if other actor is a of type ShooterCharacter
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			// Increment OverlappedItemCounts if OtherActor is a ShooterCharacter
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

// Function to trigger on End overlap with AreaSphere
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If Other Actor is detected 
	if (OtherActor)
	{
		// try cast Other Actor to ShooterCharacter to check if other actor is a of type ShooterCharacter
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			// Decrement OverlappedItemCounts if OtherActor is a ShooterCharacter
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

// Setting Item Active Stars Based on Item Rarity
void AItem::SetActiveStars()
{
	for (int32 i = 0; i <= 5; i++) //the 0 element isn't used 
	{
		ActiveStars.Add(false);
	}

	// Switch on Rarity
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	default:
		ActiveStars[1] = true;
		break;
	}
}

// Set Item Properties Based on Item State 
void AItem::SetItemProperties(EItemState State)
{
	// Switch on state
	switch (State)
	{
	case EItemState::EIS_PickUp:

		// If A PickUp
		/* Set SkeletalMesh properties: */
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetEnableGravity(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		/* Set AreaSphere properties: */
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		/*Set Collision Box properties */
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		PickUpWidget->SetVisibility(false);
		// If Equipped
		/* Set SkeletalMesh Properties*/
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetEnableGravity(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		// Set Mesh Properties
		ItemSkeletalMesh->SetSimulatePhysics(true);
		ItemSkeletalMesh->SetEnableGravity(true);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickUpWidget->SetVisibility(false);
		// If Equipped
		/* Set SkeletalMesh Properties*/
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetEnableGravity(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_PickedUp:
		PickUpWidget->SetVisibility(false);
		// If Equipped
		/* Set SkeletalMesh Properties*/
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetEnableGravity(false);
		ItemSkeletalMesh->SetVisibility(false);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
		/* TODO : Finish All Cases */

		/* FIXME Add Cases Here please */

	default:
		// If A PickUp
		/* Set SkeletalMesh properties: */
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		/* Set AreaSphere properties: */
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		/*Set Collision Box properties */
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		//Subtract  1 from the item count of the interpLocation struct 
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickUpItem(this);
	}
	//Scale Item Back to normal 
	SetActorScale3D(FVector(1.f));
	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		//Elapsed Time since started timer 
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);

		// Get Curve Value corresponding to elapsed time 
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		//Get the items initial location when the curve starts 
		FVector ItemLocation = ItemInterpStartLocation;

		//Get Location in front of the camera 
		const FVector CameraInterpLocation{GetInterpLocation()};

		//Vector for item to camera interp location x and y zeroed out 
		const FVector ItemToCamera{0.f, 0.f, (CameraInterpLocation - ItemLocation).Z};

		//Scale factor to multiply with curve value 
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{GetActorLocation()};

		//Interpolated X value 
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);
		//Interpolated Y Value 
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);

		// Set X and Y to interped values

		ItemLocation.X = InterpXValue;

		ItemLocation.Y = InterpYValue;

		//Adding Curve Value to the z component of the initial Location (scaled by DeltaZ)  
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		//Camera Rotation this frame 
		const FRotator CameraRotation{Character->GetFollowCamera()->GetComponentRotation()};

		//Camera rotation plus yaw offset 
		FRotator ItemRotation{0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f};

		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr)return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
	default:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
	}
}

void AItem::PlayPickUpSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
			}
		}
		else if (Character->ShouldPlayPickUpSound())
		{
			Character->StartPickUpSoundTimer();
			if (PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemSkeletalMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemSkeletalMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemSkeletalMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);

		EnableGlowMaterial();
	}
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0.f);
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime{};
	FVector CurveValue{};
	switch (ItemState)
	{
	case EItemState::EIS_PickUp:

		if (PulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	case EItemState::EIS_EquipInterping:
		if (InterpPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	default:
		break;
	}
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FreneslExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(
			TEXT("FreneslReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1.f);
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle item interp when in the equip interping state 
	ItemInterp(DeltaTime);

	//Get Curve values from PulseCurve and set dynamic material params
	UpdatePulse();
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_PickUp)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

// Set Item State Function that sets also the item  properties 
void AItem::SetItemState(EItemState State)
{
	//Setting the ItemState to State 
	ItemState = State;
	// Setting the properties based on the new State
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	//Store a handel to the character
	Character = Char;

	//Get array index in interpLocations with the lowest item count 
	InterpLocIndex = Character->GetInterpLocationIndex();

	//Add 1 to the Item Count for this interpLocation struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickUpSound(bForcePlaySound);
	//Store initial Location of the item 
	ItemInterpStartLocation = GetActorLocation();

	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().ClearTimer(PulseTimer);
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	// Get initial Yaw of the camera 
	const float CameraRotationYaw{Character->GetFollowCamera()->GetComponentRotation().Yaw};
	// Get inital Yaw of the item 
	const float ItemRotationYaw{GetActorRotation().Yaw};

	//Iniitial Yaw offset between camera and item 
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false;
}
