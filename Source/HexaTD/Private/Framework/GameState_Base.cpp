// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameState_Base.h"

/**
 * @brief 
 * 
 */
void AGameState_Base::CreateTeams()
{
    // TODO: Refactor for multiple Teams.
    PlayerTeams.Empty();

    FPlayerTeam PlayerTeam;
    PlayerTeam.LifeCount = 20;
    PlayerTeam.TeamID = 0;
    PlayerTeams.Add(PlayerTeam);
}

/**
 * @brief 
 * 
 * @param TeamID 
 * @param LifesCount 
 */
void AGameState_Base::TeamLosingLives(int TeamID, int LivesCount, bool& IsGameOver)
{
    if (PlayerTeams.Num())
    {
        FPlayerTeam PlayerTeam = PlayerTeams[0];
        PlayerTeam.LifeCount -= LivesCount;
        IsGameOver = PlayerTeam.LifeCount <= 0;
    }
}

// TODO: REMOVE
/**
 * @brief Called by GameMode.
 * Calls the Delegate OnMatchStateBuildPhase so clients can update accordingly.
 */
void AGameState_Base::ClientMatchStateChangedBuildPhase_Implementation()
{
    OnMatchStateBuildPhase.Broadcast();
}

// TODO: REMOVE
/**
 * @brief Called by GameMode.
 * Calls the Delegate OnMatchStateWavePhase so clients can update accordingly.
 */
void AGameState_Base::ClientMatchStateChangedWavePhase_Implementation()
{
    OnMatchStateWavePhase.Broadcast();
}
