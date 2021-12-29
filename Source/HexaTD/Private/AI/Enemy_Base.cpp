// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController.h"
#include "AI/Enemy_Base.h"
#include "AI/SpawnPoint.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BrainComponent.h"
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

	// MovementComponent
	MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	MovementComponent->FallingLateralFriction = FallingLateralFriction;
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
	if (Effect.IsPoison)
	{
		HandlePoisonEffect(Effect);
	}
	if (Effect.IsPushBack)
	{
		HandlePushBackEffect(Effect);
	}
	if (Effect.IsTeleport)
	{
		HandleTeleportEffect(Effect);
	}
}

/**
 * @brief TODO
 * 
 * @param Hit 
 */
void AEnemy_Base::Landed(const FHitResult& Hit)
{	
	Super::Landed(Hit);
	// Call HandlePostOnLanded after short delay
	GetWorldTimerManager().ClearTimer(TimerHandlePostOnLanded);
	GetWorldTimerManager().SetTimer(TimerHandlePostOnLanded, this, &AEnemy_Base::HandlePostOnLanded, PostOnLandedSlidingDelay, false);
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
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}
}


/**
 * @brief TODO
 * 
 * @param Effect 
 */
void AEnemy_Base::HandlePoisonEffect(FEffect Effect)
{
	IsPoisoned = true;
	float Interval = Effect.PoisonInterval;
	CurrentPoisonEffect = Effect;
	CurrentPoisonTickCount = 0;
	// Clear Timer in case enemy was already poisoned before
	GetWorldTimerManager().ClearTimer(TimerHandlePoisonEffect);
	GetWorldTimerManager().SetTimer(TimerHandlePoisonEffect, this, &AEnemy_Base::HandlePoisonEffectInterval, Interval, true);
}

/**
 * @brief TODO
 */
void AEnemy_Base::HandlePoisonEffectInterval()
{
	UpdateHitpoints(CurrentPoisonEffect.PoisonDamage);
	CurrentPoisonTickCount++;
	if (CurrentPoisonTickCount >= CurrentPoisonEffect.PoisonTickCount)
	{
		GetWorldTimerManager().ClearTimer(TimerHandlePoisonEffect);
		IsPoisoned = false;
	}
}

/**
 * @brief TODO
 * 
 * @param Effect 
 */
void AEnemy_Base::HandlePushBackEffect(FEffect Effect)
{
	FVector BackwardsVector = GetActorForwardVector() * Effect.PushBackForce * -1;
	FVector LaunchVector = FVector(BackwardsVector.X, BackwardsVector.Y, Effect.PushBackForce / 2);
	LaunchCharacter(LaunchVector, true, true);
	if (!AIController)
	{
		AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	}
	if (AIController)
	{
		AIController->StopMovement();
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (BrainComponent)
		{
			FString StopLogicReason;
			BrainComponent->StopLogic(StopLogicReason);
			MovementComponent->GroundFriction = 0.f;
			MovementComponent->BrakingDecelerationWalking = SlidingBrakingDecelerationWalking;
		}
	}
}

/**
 * @brief TODO
 * 
 */
void AEnemy_Base::HandlePostOnLanded()
{
	if (AIController)
	{
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (BrainComponent)
		{
			MovementComponent->GroundFriction = NormalGroundFriction;
			MovementComponent->BrakingDecelerationWalking = NormalBrakingDecelerationWalking;
			BrainComponent->StartLogic();
		}
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
