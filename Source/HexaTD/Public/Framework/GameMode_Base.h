// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameMode_Base.generated.h"

class AActor;
class AGameState_Base;
class AHexGrid;
class APlayerController_Base;
class ASpawnPoint;

/**
 * 
 */
UCLASS()
class HEXATD_API AGameMode_Base : public AGameMode
{
	GENERATED_BODY()

public:
	AGameMode_Base();

	/**
	 * Overrides
	 */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	virtual void OnMatchStateSet() override;
	bool ReadyToStartMatch();
	bool ReadyToEndMatch();

	/**
	 * Gameplay
	 */
	void HandleMatchStateInProgress();
	void HandleMatchStateBuildingPhase();
	void HandleMatchStateWavePhase();
	void HandlePlayerIsReady();
	void HandleWaveFinishedSpawn();
	UFUNCTION(BlueprintCallable)
	void TeamLosingLifes(int TeamID, int LifesCount);
	void OccupieSystemGridSpaces();
	void CheckForWaveFinished();

protected:

private:
	TArray<APlayerController_Base*> PlayerControllers;
	AGameState_Base* GameState;
	AHexGrid* HexGrid;
	// TODO: Can there be multiple?
	AActor* GoalPoint;
	// TODO: Can there be multiple?
	ASpawnPoint* SpawnPoint;
	TArray<AActor*> CheckPoints;
	bool bWaveFinishedSpawn = false;
	FName MatchStateWaitToStart = "WaitingToStart";
	FName MatchStateInProgress = "InProgress";
	FName MatchStateBuildingPhase = "BuildingPhase";
	FName MatchStateWavePhase = "WavePhase";

};

