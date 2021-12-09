// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate_NotifyPlayerStateChange);

/**
 * 
 */
UCLASS()
class HEXATD_API APlayerState_Base : public APlayerState
{
	GENERATED_BODY()

public:
	/**
	 * Replication
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSetReadyForWavePhase(bool Ready);
	UFUNCTION(Server, Reliable)
	void ServerSetReadyForWavePhase(bool Ready);
	UFUNCTION(Client, Reliable)
	void ClientAllowBuildingUpdated();
	UFUNCTION(Client, Reliable)
	void ClientResourceAmountUpdated();
	UFUNCTION()
	void OnRep_AllowBuildingUpdated();
	UFUNCTION()
	void OnRep_ResourceAmountUpdated();

	/**
	 * Getters & Setters
	 */
	UFUNCTION(BlueprintCallable)
	bool GetAllowBuilding() const;
	void SetAllowBuilding(bool Allow);
	UFUNCTION(BlueprintCallable)
	int32 GetResourceAmount() const;
	bool GetReadyForWavePhase() const;
	void SetReadyForWavePhase(bool Ready);

	/**
	 * Delegates
	 */
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyPlayerStateChange OnAllowBuildingUpdated;
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyPlayerStateChange OnResourceAmountUpdated;

	// TODO: Move to private
	UPROPERTY(ReplicatedUsing = OnRep_ResourceAmountUpdated)
	int32 ResourceAmount = 0;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_AllowBuildingUpdated)
	bool bAllowBuilding = false;
	UPROPERTY(Replicated)
	bool bReadyForWavePhase = false;
};
