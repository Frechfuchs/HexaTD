// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PlayerPawn_Base.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

/**
 * @brief Construct a new APlayerPawn_Base object
 * 
 */
APlayerPawn_Base::APlayerPawn_Base()
{
    SetActorTickEnabled(true);

    // SpringArmComponent
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->TargetArmLength = 1500.f;
    SpringArmComponent->SetWorldRotation(FRotator(-65.f, 0.f, 0.f));
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->SetupAttachment(RootComponent);

    //CameraComponent
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
}

/**
 * @brief 
 * 
 * @param PlayerInputComponent 
 */
void APlayerPawn_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerPawn_Base::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerPawn_Base::MoveRight);
}

/**
 * @brief 
 * 
 * @param AxisValue 
 */
void APlayerPawn_Base::MoveForward(float AxisValue)
{
    AddMovementInput(GetActorForwardVector(), AxisValue);
}

/**
 * @brief 
 * 
 * @param AxisValue 
 */
void APlayerPawn_Base::MoveRight(float AxisValue)
{
    AddMovementInput(GetActorRightVector(), AxisValue);
}
