// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AItem::AItem():
#pragma region Variable Initialization
	// Item Variables 
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_PickUp)

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

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Set Item State Function that sets also the item  properties 
void AItem::SetItemState(EItemState State)
{
	//Setting the ItemState to State 
	ItemState = State;
	// Setting the properties based on the new State
	SetItemProperties(State);
}
