// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AIController_Base.h"
#include "AI/Enemy_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief 
 * 
 */
void AAIController_Base::BeginPlay()
{
	Super::BeginPlay();
	// Spawn Actor as deferred, so we can set props before it gets initiliazed.
	AEnemy_Base* Enemy = Cast<AEnemy_Base>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetWorld(), EnemyClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
	if (Enemy)
	{
		// Set Props here
		UGameplayStatics::FinishSpawningActor(Enemy, SpawnTransform);
        Possess(Enemy);

		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
			UBlackboardComponent* Bb = GetBlackboardComponent();
			if (Bb)
			{
				// TODO: Refactor and check if values already set
				if (GoalPoint) Bb->SetValueAsVector(TargetLocationName, GoalPoint->GetActorLocation());
				if (CheckPointsObject) Bb->SetValueAsObject(CheckPointsObjectName, CheckPointsObject);
			}
		}
	}
}
