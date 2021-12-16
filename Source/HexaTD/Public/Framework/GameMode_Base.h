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
class UOnlineGameInstance;

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

	/**
	 * Gameplay
	 */
	void HandleMatchStateInProgress();
	void HandleMatchStateBuildingPhase();
	void HandleMatchStateWavePhase();
	void HandleMatchStateGameOver();
	void HandlePlayerIsReady();
	void HandleWaveFinishedSpawn();
	UFUNCTION(BlueprintCallable)
	void TeamLosingLives(int TeamID, int LivesCount);
	void OccupieSystemGridSpaces();
	void CheckForWaveFinished();
	void ForbidPlayersBuilding();
	bool IsBuildingPhase();
	UFUNCTION(BlueprintCallable)
	void ResetGame();

protected:
	/**
	 * Overrides
	 */
	bool ReadyToStartMatch_Implementation() override;

private:
	UOnlineGameInstance* GameInstance;
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
	FName MatchStateGameOver = "GameOver";

};

