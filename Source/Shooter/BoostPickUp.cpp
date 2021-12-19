// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostPickUp.h"

#include "NiagaraCommon.h"
#include "NiagaraComponentPool.h"
#include "ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraComponent.h"

// Sets default values
ABoostPickUp::ABoostPickUp():
	HealingAmount(100),
	BoostPickUpType(EBoostPickUpType::EBPT_HEALTH)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	SetRootComponent(PickUpMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particles"));
	NiagaraComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoostPickUp::BeginPlay()
{
	Super::BeginPlay();

	//Setup Overlap for Overlap Sphere
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoostPickUp::OnSphereOverlap);

	NiagaraComponent->GetSystemInstance()->Activate(FNiagaraSystemInstance::EResetMode::None);
}

// Called every frame
void ABoostPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoostPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* Shooter = Cast<AShooterCharacter>(OtherActor);
		if (Shooter)
		{
			if (PickUpSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, Shooter->GetActorLocation());
			}
			if (PickUpEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickUpEffect, GetActorLocation());
			}
			switch (BoostPickUpType)
			{
			case EBoostPickUpType::EBPT_HEALTH:
				Heal(Shooter, HealingAmount);
				break;
			default:
				break;
			}
			Destroy();
		}
	}
}

void ABoostPickUp::Heal(AShooterCharacter* Shooter, float HealthAmount)
{
	if (Shooter->GetHealth() >= Shooter->GetMaxHealth())return;
	if (Shooter->GetHealth() + HealthAmount <= Shooter->GetMaxHealth())
	{
		Shooter->SetHealth(Shooter->GetHealth() + HealthAmount);
	}
	else
	{
		Shooter->SetHealth(Shooter->GetMaxHealth());
	}
}
