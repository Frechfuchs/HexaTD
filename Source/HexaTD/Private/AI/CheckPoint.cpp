// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CheckPoint.h"
#include "Components/ArrowComponent.h"

/**
 * @brief Construct a new ACheckPoint object
 * 
 */
ACheckPoint::ACheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// ArrowComponent
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetRelativeRotation(FRotator(270.f, 0.f, 0.f));
	ArrowComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	ArrowComponent->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));	ArrowComponent->SetArrowColor(FColor::Yellow);
	ArrowComponent->SetHiddenInGame(false);

}

/**
 * @brief Called when the game starts or when spawned
 * 
 */
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
}
