// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Enemy_Base.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class AGameMode_Base;
class AGameState_Base;
class UArrowComponent;
class UBehaviorTree;
class UCheckPointsObject;

UCLASS()
class HEXATD_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

	// Called by GameMode to start next wave
	UFUNCTION(BlueprintCallable)
	void StartWave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void SpawnAI();
	UPROPERTY()
	UArrowComponent* ArrowComponent;
	UPROPERTY()
	AGameMode_Base* GameMode;
	UPROPERTY()
	AGameState_Base* GameState;
	UPROPERTY(EditInstanceOnly)
	TSubclassOf<AEnemy_Base> EnemyClass = AEnemy_Base::StaticClass();
	UPROPERTY(EditInstanceOnly)
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditInstanceOnly)
	UDataTable* EnemyWaveTable;
	UPROPERTY()
	AActor* GoalPoint;
	UPROPERTY()
	UCheckPointsObject* CheckPointsObject;
	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
	UPROPERTY()
	int32 SpawnArrayElementIndex;
	UPROPERTY()
	int32 SpawnElementCountIndex;

};
