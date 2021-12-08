// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy_Base.h"
#include "Framework/GameMode_Base.h"
#include "UI/HealthbarComponent.h"

/**
 * @brief Construct a new aenemy base::aenemy base object
 * 
 */
AEnemy_Base::AEnemy_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Healthbar
	Healthbar = CreateDefaultSubobject<UHealthbarComponent>(TEXT("HealthbarComponent"));
	Healthbar->SetupAttachment(RootComponent);

}

/**
 * @brief Called when the game starts or when spawned
 * 
 */
void AEnemy_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) GameMode = Cast<AGameMode_Base>(GetWorld()->GetAuthGameMode());
	
	RestoreHealth();
}

/**
 * @brief Called every frame
 * 
 * @param DeltaTime 
 */
void AEnemy_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
 * @brief TODO
 * 
 */
void AEnemy_Base::UpdateHealthbar()
{
	if (Healthbar) Healthbar->SetHitpointsPercent(CurrentHitpoints / MaxHitpoints);
}

/**
 * @brief TODO
 * 
 */
void AEnemy_Base::RestoreHealth()
{
	CurrentHitpoints = MaxHitpoints;
	UpdateHealthbar();
}

void AEnemy_Base::Effect_Implementation(FEffect Effect)
{
	UpdateHitpoints(Effect.Damage);
}

void AEnemy_Base::UpdateHitpoints(float Damage)
{
	CurrentHitpoints -= Damage;
	UpdateHealthbar();
	CheckForDeath();
}

void AEnemy_Base::CheckForDeath()
{
	if (HasAuthority())
	{
		if (CurrentHitpoints <= 0.f)
		{
			Destroy();
			if (GameMode) GameMode->CheckForWaveFinished();
		}
	}
}
