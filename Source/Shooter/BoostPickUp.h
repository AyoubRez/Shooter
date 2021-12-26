// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostPickUp.generated.h"

class AShooterCharacter;
UENUM(BlueprintType)
enum class EBoostPickUpType:uint8
{
	EBPT_HEALTH UMETA(DisplayName="Health"),


	EBPT_MAX UMETA(DisplayName="Default Max")
};

UCLASS()
class SHOOTER_API ABoostPickUp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABoostPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh", meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* PickUpMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh", meta=(AllowPrivateAccess="true"))
	class USphereComponent* OverlapSphere;

	/** Type of the boost Pick up  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boost", meta=(AllowPrivateAccess="true"))
	EBoostPickUpType BoostPickUpType;

	/** Health boost */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boost", meta=(AllowPrivateAccess="true"))
	float HealingAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound", meta=(AllowPrivateAccess="true"))
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect", meta=(AllowPrivateAccess="true"))
	class UNiagaraSystem* PickUpIdleParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect", meta=(AllowPrivateAccess="true"))
	UNiagaraSystem* PickUpEffect;


protected:
	// Called When Overlapping Area sphere 
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Heal(AShooterCharacter* Target, float HealthAmount);
};
