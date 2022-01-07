// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Building_Base.h"
#include "AI/Enemy_Base.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/Material.h"
#include "Net/UnrealNetwork.h"

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
	TargetCollisionComponent->SetSphereRadius(Range);
	TargetCollisionComponent->SetVisibility(false);
	TargetCollisionComponent->SetHiddenInGame(false);
	TargetCollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FScriptDelegate BeginOverlapDelegate;
	BeginOverlapDelegate.BindUFunction(this, "OnTargetCollisionBeginOverlap");
	TargetCollisionComponent->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	FScriptDelegate EndOverlapDelegate;
	EndOverlapDelegate.BindUFunction(this, "OnTargetCollisionEndOverlap");
	TargetCollisionComponent->OnComponentEndOverlap.Add(EndOverlapDelegate);
	TargetCollisionComponent->SetupAttachment(RootComponent);

	// DecalComponent
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterial(TEXT("Material'/Game/HexaTD/Materials/System/M_RangeDecal.M_RangeDecal'"));
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeIndicator"));
	DecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComponent->SetVisibility(false);
	if (DecalMaterial.Object)
	{
    	UMaterial* RangeIndicatorMaterial = Cast<UMaterial>(DecalMaterial.Object);
		DecalComponent->SetDecalMaterial(RangeIndicatorMaterial);
		DecalComponent->DecalSize = FVector(Range, Range, Range);
	}
	DecalComponent->SetupAttachment(RootComponent);

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
		// TODO: Remove one of the two commands here (which ever works stays)
		PrimaryActorTick.bCanEverTick = false;
		SetActorTickEnabled(false);
		bReplicates = false;
		bCanUseEffect = false;

		BodyCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TargetCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DecalComponent->SetVisibility(true);
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

	if (HasAuthority() && bCanUseEffect && HasTarget() && !bIsPreview)
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
 */
void ABuilding_Base::Upgrade()
{
	Level++;
	Effect.Damage += Effect.Damage * 1.5f;
	// broadcast for host
	OnLevelChanged.Broadcast();
}

/**
 * @brief TODO
 * 
 * @param OutLifetimeProps 
 */
void ABuilding_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding_Base, Level);
}

/**
 * @brief 
 * 
 */
void ABuilding_Base::ClientOnLevelChanged_Implementation()
{
	OnLevelChanged.Broadcast();
}

/**
 * @brief TODO
 * 
 */
void ABuilding_Base::OnRep_LevelChanged()
{
	ClientOnLevelChanged();
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

/**
 * @brief TODO
 * 
 * @return int32 
 */
int32 ABuilding_Base::GetUpgradeCost() const
{
	return (int32)FGenericPlatformMath::Pow((float)Level, 2.f) + 1;
}

/**
 * @brief TODO
 * 
 * @return int32 
 */
int32 ABuilding_Base::GetLevel() const
{
	return Level;
}

/**
 * @brief TODO
 * 
 * @param Visibility 
 */
void ABuilding_Base::SetDecalVisibility(bool Visibility)
{
	DecalComponent->SetVisibility(Visibility);
}

/**
 * @brief TODO
 * 
 * @param OverlappedComp 
 * @param OtherActor 
 * @param OtherComp 
 * @param OtherBodyIndex 
 * @param bFromSweep 
 * @param SweepResult 
 */
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

/**
 * @brief TODO
 * 
 * @param OverlappedComp 
 * @param OtherActor 
 * @param OtherComp 
 * @param OtherBodyIndex 
 * @param bFromSweep 
 * @param SweepResult 
 */
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

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool ABuilding_Base::HasTarget() const
{
	return Targets.Num() > 0;
}

/**
 * @brief TODO
 * 
 * @return TArray<AEnemy_Base*> 
 */
void ABuilding_Base::GetTargets_Implementation(TArray<AEnemy_Base*> &InTargets) const
{
	// Get first element in Targets
	InTargets.Add(Targets[0]);
}

/**
 * @brief TODO
 */
void ABuilding_Base::UseEffect()
{
	TArray<AEnemy_Base*> LocTargets;
	GetTargets(LocTargets);
	// TODO: Refactor
	for (AEnemy_Base* Target : LocTargets)
	{
		if (Target->GetClass()->ImplementsInterface(UEffectable::StaticClass()))
		{
			IEffectable::Execute_Effect(Target, Effect);
		}
	}
}

/**
 * @brief TODO
 * 
 */
void ABuilding_Base::ResetEffectDelay()
{
	bCanUseEffect = true;
}
