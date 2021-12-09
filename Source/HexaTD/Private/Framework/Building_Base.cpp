// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Building_Base.h"
#include "AI/Enemy_Base.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

/**
 * @brief Construct a new ABuilding_Base object
 * 
 */
ABuilding_Base::ABuilding_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// BodyCollision
	BodyCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BodyCollision"));
	BodyCollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	BodyCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BodyCollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	RootComponent = BodyCollisionComponent;

	// StaticMeshComponent
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetupAttachment(RootComponent);

	//TargetCollisionComponent
	TargetCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RangeCollision"));
	TargetCollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TargetCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TargetCollisionComponent->SetSphereRadius(300.f);
	TargetCollisionComponent->SetVisibility(true);
	TargetCollisionComponent->SetHiddenInGame(false);
	TargetCollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FScriptDelegate BeginOverlapDelegate;
	BeginOverlapDelegate.BindUFunction(this, "OnTargetCollisionBeginOverlap");
	TargetCollisionComponent->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	FScriptDelegate EndOverlapDelegate;
	EndOverlapDelegate.BindUFunction(this, "OnTargetCollisionEndOverlap");
	TargetCollisionComponent->OnComponentEndOverlap.Add(EndOverlapDelegate);
	TargetCollisionComponent->SetupAttachment(RootComponent);

	// Props
	bReplicates = true;
	bCanUseEffect = true;

	// TODO: Move
	Effect.Damage = 10.f;
}

/**
 * @brief Called when the game starts or when spawned
 * 
 */
void ABuilding_Base::BeginPlay()
{
	Super::BeginPlay();

	// In our constructor bIsPreview is not set yet, so we need to put preview-related code here:
	if (bIsPreview)
	{
		PrimaryActorTick.bCanEverTick = false;
		bReplicates = false;
		bCanUseEffect = false;

		BodyCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TargetCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

/**
 * @brief Called every frame
 * 
 * @param DeltaTime 
 */
void ABuilding_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (bIsPreview) UE_LOG(LogTemp, Error, TEXT("I SHOULD NOT BE TICKING!"));
	if (bCanUseEffect && HasTarget())
	{
		bCanUseEffect = false;
		UseEffect();

		// Add delay for bCanUseEffect to be true again
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ABuilding_Base::ResetEffectDelay, EffectDelay, false);
	}
}

/**
 * @brief TODO
 * 
 * @param IsPreview 
 */
void ABuilding_Base::SetIsPreview(bool IsPreview)
{
	bIsPreview = IsPreview;
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool ABuilding_Base::GetIsPreview() const
{
	return bIsPreview;
}

/**
 * @brief TODO
 * 
 * @param Material 
 */
void ABuilding_Base::SetMeshMaterial(UMaterialInterface* Material)
{
	for (int i = 0; i < StaticMeshComponent->GetNumMaterials(); i++)
	{
		StaticMeshComponent->SetMaterial(i, Material);
	}
}

void ABuilding_Base::OnTargetCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UEffectable::StaticClass()))
	{
		AEnemy_Base* Enemy = Cast<AEnemy_Base>(OtherActor);
		if (Enemy)
		{
			// Add to Targets
			Targets.AddUnique(Enemy);
		}
	}
}

void ABuilding_Base::OnTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UEffectable::StaticClass()))
	{
		AEnemy_Base* Enemy = Cast<AEnemy_Base>(OtherActor);
		if (Enemy)
		{
			// Remove from Targets
			Targets.Remove(Enemy);
		}
	}
}

bool ABuilding_Base::HasTarget() const
{
	return Targets.Num() > 0;
}

AEnemy_Base* ABuilding_Base::GetTarget() const
{
	// TODO: Implement targeting
	return Targets[0];
}

void ABuilding_Base::UseEffect()
{
	AEnemy_Base* Target = GetTarget();
	// TODO: Refactor
	if (Target->GetClass()->ImplementsInterface(UEffectable::StaticClass()))
	{
		IEffectable::Execute_Effect(Target, Effect);
	}
}

void ABuilding_Base::ResetEffectDelay()
{
	bCanUseEffect = true;
}
