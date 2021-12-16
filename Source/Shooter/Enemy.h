// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletHitInterface.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();

	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void Die();

	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	void ResetHitReactTimer();

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	// Called when something overlaps with the agro sphere
	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	// Called when something overlaps with the attack sphere
	UFUNCTION()
	void AttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp,
	                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when something overlaps with the attack sphere
	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex);

	// Called to play an attack montage 
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate = 1.0f);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	// Called when something overlaps with the left weapon box
	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when something overlaps with the right weapon box
	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Activate / Deactivate Collision for weapons  
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeapon();

	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeapon();

	void DoDamage(AActor* Victime);

private:
	/** Particles to spawn when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UParticleSystem* ImpactParticles;

	/** Sound to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class USoundCue* ImpactSound;

	/** The Health of this actor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	float Health;

	/** The Max Health of this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float MaxHealth;

	/** The Bone Name for the head  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	FString HeadBone;

	/** Time to display the health bar before toggling it   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float HealthBarDisplayTime;

	FTimerHandle HealthBarTimer;

	/** Animation Montage Hit and Death Animation   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UAnimMontage* HitMontage;


	FTimerHandle HitReactTimer;

	/** Min time to disable hit react    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float HitReactTimeMin;

	/** Max time to disable Hit React    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float HitReactTimeMax;

	bool bCanHitReact;

	/** Map to store hit number widget  and there locations */
	UPROPERTY(VisibleAnywhere, Category="Combat", meta=(AllowPrivateAccess="true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	/** Time before remove a hit number from screen    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float HitNumberDestroyTime;

	/** True when in attack range    */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	bool bInAttackRange;

#pragma region AI

	/** Behavior Tree for the AI Character    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior Tree ", meta=(AllowPrivateAccess="true"))
	class UBehaviorTree* BehaviorTree;

	/** Point for the enemy to move to    */
	UPROPERTY(EditAnywhere, Category="Behavior Tree ", meta=(AllowPrivateAccess="true", MakeEditWidget="true"))
	FVector PatrolPoint;

	class AEnemyController* EnemyController;

	/** Second Point for the enemy to move to    */
	UPROPERTY(EditAnywhere, Category="Behavior Tree ", meta=(AllowPrivateAccess="true", MakeEditWidget="true"))
	FVector PatrolPointTwo;

	/** Overlap Sphere for when the enemy becomes hostel    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class USphereComponent* AgroSphere;

	/** True when Playing the get hit animation    */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	bool bStunned;


	/** Chance of being stunned between 0 and 1    */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float StunChance;

	/**  Sphere for Attack Range   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	USphereComponent* AttackRangeSphere;

	/** Attack Montage  Animation   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UAnimMontage* AttackMontage;

	/** The Four attack montage Section names */
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;

	/** Collision Volume for the left weapon     */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UBoxComponent* LeftWeaponCollision;

	/** Collision Volume for the Right weapon    */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UBoxComponent* RightWeaponCollision;

	/** Base damage for enemy   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float BaseDamage;

#pragma endregion

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Implementation of the bulletHitFunction inherited from interface  */
	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
