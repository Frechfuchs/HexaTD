// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Effectable.h"
#include "Enemy_Base.generated.h"

class UHealthbarComponent;
class AGameMode_Base;
class AAIController;

UCLASS()
class HEXATD_API AEnemy_Base : public ACharacter, public IEffectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy_Base();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Replication
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void OnRep_CurrentHitpointsUpdated();

	// From IEffectable
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
	void Effect(FEffect& Effect);
	virtual void Effect_Implementation(FEffect Effect) override;
	/** Handles the logic after push effect */
	virtual void Landed(const FHitResult& Hit) override;

	// TODO: Wrap with Getter & Setter
	UPROPERTY(BlueprintReadOnly)
	bool IsSlowed = false;
	UPROPERTY(BlueprintReadOnly)
	bool IsPoisoned = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UHealthbarComponent* Healthbar;
	UPROPERTY(EditDefaultsOnly)
	float MaxHitpoints = 100;
	UPROPERTY(EditDefaultsOnly)
	float MaxWalkSpeed = 600.f;

private:
	void UpdateHealthbar();
	void RestoreHealth();
	void UpdateHitpoints(float Damage);
	void HandleSlowEffect(FEffect Effect);
	void HandleRemoveSlowEffect();
	void HandlePoisonEffect(FEffect Effect);
	void HandlePoisonEffectInterval();
	void HandlePushBackEffect(FEffect Effect);
	void HandlePostOnLanded();
	void HandleTeleportEffect(FEffect Effect);
	void CheckForDeath();

	AGameMode_Base* GameMode;
	UCharacterMovementComponent* MovementComponent;
	AAIController* AIController;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHitpointsUpdated)
	float CurrentHitpoints = 1;
	FTimerHandle TimerHandleSlowEffect;
	FTimerHandle TimerHandlePoisonEffect;
	FTimerHandle TimerHandlePostOnLanded;
	FEffect CurrentPoisonEffect;
	int32 CurrentPoisonTickCount = 0;
	float FallingLateralFriction = 0.3f;
	float NormalGroundFriction = 8.f;
	float NormalBrakingDecelerationWalking = 2048.f;
	float SlidingBrakingDecelerationWalking = 350.f;
	float PostOnLandedSlidingDelay = 0.7f;
};
