// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Effectable.h"
#include "Enemy_Base.generated.h"

class UHealthbarComponent;
class AGameMode_Base;

UCLASS()
class HEXATD_API AEnemy_Base : public ACharacter, public IEffectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy_Base();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// From IEffectable
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
	void Effect(FEffect& Effect);
	virtual void Effect_Implementation(FEffect Effect) override;

	// TODO: Wrap with Getter & Setter
	UPROPERTY(BlueprintReadOnly)
	bool IsSlowed = false;
	UPROPERTY(BlueprintReadOnly)
	bool IsPoisoned = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
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
	void HandleTeleportEffect(FEffect Effect);
	void CheckForDeath();

	float CurrentHitpoints = 1;
	AGameMode_Base* GameMode;
	FTimerHandle TimerHandleSlowEffect;
	FTimerHandle TimerHandlePoisonEffect;
	FEffect CurrentPoisonEffect;
	int32 CurrentPoisonTickCount = 0;
};
