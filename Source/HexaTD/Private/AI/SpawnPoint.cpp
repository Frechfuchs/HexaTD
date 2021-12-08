// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SpawnPoint.h"
#include "AI/AIController_Base.h"
#include "AI/CheckPoint.h"
#include "AI/GoalPoint.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Framework/GameMode_Base.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Construct a new ASpawnPoint object
 * 
 */
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// RootComponent
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	// ArrowComponent
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetRelativeRotation(FRotator(270.f, 0.f, 0.f));
	ArrowComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	ArrowComponent->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));	ArrowComponent->SetArrowColor(FColor::Green);
	ArrowComponent->SetHiddenInGame(false);
	ArrowComponent->SetupAttachment(RootComponent);
}

/**
 * @brief TODO
 * 
 */
void ASpawnPoint::StartWave()
{
	SpawnAI();
	if (GameMode) GameMode->HandleWaveFinishedSpawn();
}

/**
 * @brief Called when the game starts or when spawned
 * 
 */
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	GameMode = World->GetAuthGameMode<AGameMode_Base>();
	GoalPoint = UGameplayStatics::GetActorOfClass(World, AGoalPoint::StaticClass());
	
	// Creeate CheckPointsObject
	CheckPointsObject = NewObject<UCheckPointsObject>(this);
	TArray<AActor*> CheckPointActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACheckPoint::StaticClass(), CheckPointActors);
	for (AActor* CheckPointActor : CheckPointActors)
	{
		CheckPointsObject->CheckPoints.Add(CheckPointActor->GetActorLocation());
	}
}

/**
 * @brief TODO
 * 
 */
void ASpawnPoint::SpawnAI()
{
	FTransform SpawnTransform = GetActorTransform();
	// Spawn Actor as deferred, so we can set props before it gets initiliazed.
	AAIController_Base* AIC = Cast<AAIController_Base>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetWorld(), AAIController_Base::StaticClass(), SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
	if (AIC)
	{
		// Set Props here
		AIC->SpawnTransform = SpawnTransform;
		AIC->EnemyClass = EnemyClass;
		if (BehaviorTree) AIC->BehaviorTree = BehaviorTree;
		if (GoalPoint) AIC->GoalPoint = GoalPoint;
		if (CheckPointsObject) AIC->CheckPointsObject = CheckPointsObject;
		// TODO: WaveTable?
		UGameplayStatics::FinishSpawningActor(AIC, SpawnTransform);
	}
}
