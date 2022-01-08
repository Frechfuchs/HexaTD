// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Effectable.h"
#include "Building_Base.generated.h"

USTRUCT(Blueprintable)
struct HEXATD_API FBuildingUpgrade {
	GENERATED_BODY()

	// TODO: Make a tree here?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffect Option_A;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffect Option_B;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffect Option_C;
};

class AEnemy_Base;
class UDecalComponent;
class UMaterialInterface;
class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate_NotifyBuildingChange);

UCLASS()
class HEXATD_API ABuilding_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding_Base();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Gameplay
	 */
	void Upgrade(int32 UpgradeIndex);

	/**
	 * Replication
	 */
	/** TODO */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientOnLevelChanged();
	/** TODO */
	UFUNCTION()
	void OnRep_LevelChanged();
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientOnEffectChanged();
	/** TODO */
	UFUNCTION()
	void OnRep_EffectChanged();
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientOnUpgradesChanged();
	/** TODO */
	UFUNCTION()
	void OnRep_UpgradesChanged();

	/**
	 * Getters & Setters
	 */
	/** TODO */
	void SetIsPreview(bool IsPreview);
	/** TODO */
	UFUNCTION(BlueprintCallable)
	bool GetIsPreview() const;
	/** TODO */
	void SetMeshMaterial(UMaterialInterface* Material);
	/** TODO */
	UFUNCTION(BlueprintCallable)
	int32 GetUpgradeCost() const;
	/** TODO */
	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const;
	/** TODO */
	void SetDecalVisibility(bool Visibility);

	/**
	 * Delegates
	 */
	FDelegate_NotifyBuildingChange OnLevelChanged;
	FDelegate_NotifyBuildingChange OnEffectChanged;
	FDelegate_NotifyBuildingChange OnUpgradesChanged;

	/** TODO */
	UPROPERTY()
	bool bIsPreview = false;
	/** TODO */
	UPROPERTY(BlueprintReadWrite)
	int32 ResourceCost = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(TArray<AEnemy_Base*> &InTargets) const;

	/** TODO */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_EffectChanged)
	FEffect Effect;
	/** TODO */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_UpgradesChanged)
	FBuildingUpgrade Upgrades;
	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(BlueprintReadWrite)
	USphereComponent* BodyCollisionComponent;
	// TODO: Move to TargetComponent?
	USphereComponent* TargetCollisionComponent;
	UDecalComponent* DecalComponent;
	UPROPERTY(ReplicatedUsing = OnRep_LevelChanged)
	int32 Level = 1;
	UPROPERTY(EditDefaultsOnly)
	float Range = 300.f;
	UPROPERTY(BlueprintReadOnly)
	TArray<AEnemy_Base*> Targets;

private:
	UFUNCTION()
	void OnTargetCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	bool HasTarget() const;
	UFUNCTION()
	void UseEffect();
	UFUNCTION()
	void ResetEffectDelay();

	UPROPERTY()
	bool bCanUseEffect = false;
};
