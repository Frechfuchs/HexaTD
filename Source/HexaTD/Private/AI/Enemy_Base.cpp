// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy_Base.h"
#include "AI/SpawnPoint.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Framework/GameMode_Base.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
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

	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
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

/**
 * @brief TODO
 * 
 * @param Effect 
 */
void AEnemy_Base::Effect_Implementation(FEffect Effect)
{
	UpdateHitpoints(Effect.Damage);

	if (Effect.IsSlow)
	{
		HandleSlowEffect(Effect);
	}
	if (Effect.IsTeleport)
	{
		HandleTeleportEffect(Effect);
	}
}

/**
 * @brief TODO
 * 
 * @param Damage 
 */
void AEnemy_Base::UpdateHitpoints(float Damage)
{
	CurrentHitpoints -= Damage;
	UpdateHealthbar();
	CheckForDeath();
}

/**
 * @brief TODO
 * 
 * @param Effect 
 */
void AEnemy_Base::HandleSlowEffect(FEffect Effect)
{
	if (!IsSlowed)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (MovementComponent)
		{
			MovementComponent->MaxWalkSpeed = MaxWalkSpeed * (1.f - Effect.SlowPercent);
		}
	}
	IsSlowed = true;
	float Delay = Effect.SlowTime;
	// Clear Timer in case enemy was already slowed before
	GetWorldTimerManager().ClearTimer(TimerHandleSlowEffect);
	GetWorldTimerManager().SetTimer(TimerHandleSlowEffect, this, &AEnemy_Base::HandleRemoveSlowEffect, Delay, false);
}

/**
 * @brief TODO
 */
void AEnemy_Base::HandleRemoveSlowEffect()
{
	IsSlowed = false;
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}
}

/**
 * @brief TODO
 */
void AEnemy_Base::HandleTeleportEffect(FEffect Effect)
{
	if (Effect.TeleportType == EffectTeleportType::Spawn)
	{
		// TODO: When having multiple SpawnPoints, choose the one this Enemy spawned from
		AActor* SpawnPoint = UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnPoint::StaticClass());
		if (SpawnPoint)
		{
			SetActorLocation(SpawnPoint->GetActorLocation());
		}
	}
}

/**
 * @brief TODO
 */
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
