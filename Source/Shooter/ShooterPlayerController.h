// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

private:
	/** Reference to the overall HUD Overlay Blueprint Class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	/** Variable to hold the HUD overlay widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta=(AllowPrivateAccess="true"))
	UUserWidget* HUDOverlay;

protected:
	virtual void BeginPlay() override;
};
