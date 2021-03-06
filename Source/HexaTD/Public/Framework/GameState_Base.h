// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameState_Base.generated.h"

class APlayerState_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate_NotifyGameStateChange);

UENUM(BlueprintType)
enum GameOverReasonType
{
  None			UMETA(DisplayName = "None"),
  EnemyWon		UMETA(DisplayName = "EnemyWon"),
  TeamWon		UMETA(DisplayName = "TeamWon"),
};

USTRUCT(BlueprintType)
struct HEXATD_API FPlayerTeam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TeamID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LifeCount = 20;
	// TODO: Members
};

/**
 * 
 */
UCLASS()
class HEXATD_API AGameState_Base : public AGameState
{
	GENERATED_BODY()

public:
	/**
	 * Gameplay
	 */
	/** TODO */
	void CreateTeams();
	/** TODO */
	void TeamLosingLives(int TeamID, int LivesCount, bool& IsGameOver);
	/** TODO */
	void SetTeamLives(int TeamID, int LivesCount);
	/**
	 * Replication
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(Client, Reliable)
	void ClientGameOverUpdated();
	UFUNCTION(Client, Reliable)
	void ClientPlayerTeamsUpdated();
	UFUNCTION(Client, Reliable)
	void ClientWaveCountUpdated();
	UFUNCTION()
	void OnRep_GameOverUpdated();
	UFUNCTION()
	void OnRep_PlayerTeamsUpdated();
	UFUNCTION()
	void OnRep_WaveCountUpdated();

	/**
	 * Getters & Setters
	 */
	UFUNCTION(BlueprintCallable)
	int32 GetWaveCount() const;
	UFUNCTION(BlueprintCallable)
	TArray<FPlayerTeam> GetPlayerTeams() const;
	UFUNCTION()
	void SetGameOverReason(TEnumAsByte<GameOverReasonType> GameOver);
	UFUNCTION(BlueprintCallable)
	TEnumAsByte<GameOverReasonType> GetGameOverReason() const;

	/**
	 * Delegates
	 */
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyGameStateChange OnGameOverUpdated;
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyGameStateChange OnPlayerTeamsUpdated;
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyGameStateChange OnWaveCountUpdated;

	/** Array of all PlayerStates, maintained on both server and clients (PlayerStates are always relevant) */
	TArray<APlayerState_Base*> PlayerArray;
	/** TODO */
	UPROPERTY(ReplicatedUsing = OnRep_WaveCountUpdated)
	int32 WaveCount = 0;

protected:
	/** TODO */
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeamsUpdated)
	TArray<FPlayerTeam> PlayerTeams;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_GameOverUpdated)
	TEnumAsByte<GameOverReasonType> GameOverReason = GameOverReasonType::None;
};
