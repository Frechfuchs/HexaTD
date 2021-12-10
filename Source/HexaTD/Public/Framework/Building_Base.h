// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Effectable.h"
#include "Building_Base.generated.h"

class AEnemy_Base;
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
	void Upgrade();

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

	/**
	 * Getters & Setters
	 */
	void SetIsPreview(bool IsPreview);
	UFUNCTION(BlueprintCallable)
	bool GetIsPreview() const;
	void SetMeshMaterial(UMaterialInterface* Material);
	UFUNCTION(BlueprintCallable)
	int32 GetUpgradeCost() const;
	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const;

	/**
	 * Delegates
	 */
	FDelegate_NotifyBuildingChange OnLevelChanged;

	/** TODO */
	UPROPERTY(EditAnywhere)
	bool bIsPreview = false;
	/** TODO */
	UPROPERTY(EditDefaultsOnly)
	int32 ResourceCost = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly)
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditInstanceOnly)
	USphereComponent* BodyCollisionComponent;
	// TODO: Move to TargetComponent?
	USphereComponent* TargetCollisionComponent;
	UPROPERTY(EditDefaultsOnly)
	FEffect Effect;
	UPROPERTY(ReplicatedUsing = OnRep_LevelChanged)
	int32 Level = 1;

private:
	UFUNCTION()
	void OnTargetCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTargetCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	bool HasTarget() const;
	UFUNCTION()
	AEnemy_Base* GetTarget() const;
	UFUNCTION()
	void UseEffect();
	UFUNCTION()
	void ResetEffectDelay();

	UPROPERTY()
	bool bCanUseEffect = false;
	UPROPERTY()
	TArray<AEnemy_Base*> Targets;
	UPROPERTY()
	float EffectDelay = 1.f;
};
