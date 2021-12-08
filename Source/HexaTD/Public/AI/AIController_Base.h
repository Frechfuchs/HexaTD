// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Enemy_Base.h"
#include "AI/SpawnPoint.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController_Base.generated.h"

class AEnemy_Base;
class AGoalPoint;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class HEXATD_API AAIController_Base : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FTransform SpawnTransform;
	UPROPERTY()
	TSubclassOf<AEnemy_Base> EnemyClass = AEnemy_Base::StaticClass();
	UPROPERTY()
	UBehaviorTree* BehaviorTree;
	UPROPERTY()
	AActor* GoalPoint;
	UPROPERTY()
	UCheckPointsObject* CheckPointsObject;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

private:
	FName TargetLocationName = "TargetLocation";
	FName CheckPointsObjectName = "CheckPointsObject";
};
