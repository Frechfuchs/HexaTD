// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameState_Base.generated.h"

USTRUCT()
struct HEXATD_API FPlayerTeam
{
	GENERATED_BODY()

	int TeamID = 0;
	//Members
	int LifeCount = 20;
};

class APlayerState_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate_OnMatchStateChange);

/**
 * 
 */
UCLASS()
class HEXATD_API AGameState_Base : public AGameState
{
	GENERATED_BODY()

public:
	/**
	 * Overrides
	 */
	/* virtual void HandleBeginPlay() override;
	virtual void OnRep_MatchState() override; */
	// GetDefaultGameMode() -> returns probably correct GameMode Object

	/**
	 * Gameplay
	 */
	/* void BindPlayerReadyDelegates(); */
	/* void OnChangePlayerReady(); */
	bool IsReadyToStartWavePhase();
	/**
	 * @brief Called by GameMode.
	 * Calls the Delegate OnMatchStateBuildPhase so clients can update accordingly.
	 */
	UFUNCTION(Client, Reliable)
	void ClientMatchStateChangedBuildPhase();

	/**
	 * @brief Called by GameMode.
	 * Calls the Delegate OnMatchStateWavePhase so clients can update accordingly.
	 */
	UFUNCTION(Client, Reliable)
	void ClientMatchStateChangedWavePhase();

	/**
	 * Delegates
	 */
	UPROPERTY(BlueprintAssignable)
	FDelegate_OnMatchStateChange OnMatchStateBuildPhase;
	UPROPERTY(BlueprintAssignable)
	FDelegate_OnMatchStateChange OnMatchStateWavePhase;

	/** Array of all PlayerStates, maintained on both server and clients (PlayerStates are always relevant) */
	TArray<APlayerState_Base*> PlayerArray;

protected:
	TArray<FPlayerTeam> PlayerTeams;

};
