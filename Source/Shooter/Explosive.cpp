// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AExplosive::AExplosive():
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	Damage(100.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosive Mesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
}

void AExplosive::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AExplosive::HideHealthBar, HealthBarDisplayTime);
}

void AExplosive::Explode(AActor* Shooter, AController* ShooterController)
{
	HideHealthBar();

	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}

	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, GetActorLocation(), FRotator(90.f),
		                                         true);
	}

	TArray<AActor*> OverlappingActors;
	//TArray<AActor*> OverlappingExplosives;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
	//GetOverlappingActors(OverlappingExplosives, StaticClass());

	for (auto Actor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(Actor, Damage, ShooterController, Shooter, UDamageType::StaticClass());
	}
	/*for (auto Explo : OverlappingExplosives)
	{

		//UGameplayStatics::ApplyDamage(Explo, Damage, ShooterController, Shooter, UDamageType::StaticClass());
		//TODO Investigate and implement chaine explosion
	}*/

	Destroy();
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	IBulletHitInterface::BulletHit_Implementation(HitResult, Shooter, ShooterController);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	ShowHealthBar();
}

float AExplosive::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Explode(DamageCauser, EventInstigator);
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}
