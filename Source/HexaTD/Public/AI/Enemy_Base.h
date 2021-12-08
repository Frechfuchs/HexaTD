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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float MaxHitpoints = 100;
	UPROPERTY(EditAnywhere)
	UHealthbarComponent* Healthbar;

private:
	void UpdateHealthbar();
	void RestoreHealth();
	void UpdateHitpoints(float Damage);
	void CheckForDeath();

	float CurrentHitpoints = 1;
	AGameMode_Base* GameMode;
};
