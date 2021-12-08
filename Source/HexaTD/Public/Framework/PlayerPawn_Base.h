// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "PlayerPawn_Base.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class HEXATD_API APlayerPawn_Base : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	APlayerPawn_Base();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

private:
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComponent;
	UCameraComponent* CameraComponent;
};
