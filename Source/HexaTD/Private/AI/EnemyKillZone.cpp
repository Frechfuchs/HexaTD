// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/EnemyKillZone.h"
#include "AI/Enemy_Base.h"
#include "Components/BoxComponent.h"
#include "Framework/GameMode_Base.h"

// Sets default values
AEnemyKillZone::AEnemyKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FScriptDelegate BeginOverlapDelegate;
	BeginOverlapDelegate.BindUFunction(this, "OnBoxCollisionBeginOverlap");
	BoxComponent->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	RootComponent = BoxComponent;
}

void AEnemyKillZone::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("TRIGGERED"));
	AEnemy_Base* Enemy = Cast<AEnemy_Base>(OtherActor);
	if (Enemy)
	{
		Enemy->Destroy();
		if (!GameMode)
		{
			GameMode = Cast<AGameMode_Base>(GetWorld()->GetAuthGameMode());
		}
		if (GameMode)
		{
			GameMode->CheckForWaveFinished();
		}
	}
}
