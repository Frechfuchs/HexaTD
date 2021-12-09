// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameState_Base.h"
#include "Net/UnrealNetwork.h"

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
 * @brief TODO
 * 
 * @param TeamID 
 * @param LifesCount 
 */
// TODO: Multiple teams
void AGameState_Base::TeamLosingLives(int TeamID, int LivesCount, bool& IsGameOver)
{
    if (PlayerTeams.Num())
    {
        FPlayerTeam* PlayerTeam = &PlayerTeams[0];
        PlayerTeam->LifeCount -= LivesCount;
        IsGameOver = PlayerTeam->LifeCount <= 0;
    }
}

/**
 * @brief TODO
 * 
 * @param TeamID 
 * @param LivesCount 
 */
// TODO: Refactor for reuse in TeamLosingLives
// TODO: Multiple teams
void AGameState_Base::SetTeamLives(int TeamID, int LivesCount)
{
    if (PlayerTeams.Num())
    {
        FPlayerTeam* PlayerTeam = &PlayerTeams[0];
        PlayerTeam->LifeCount = LivesCount;
    }
}

/**
 * @brief TODO
 * 
 * @param OutLifetimeProps 
 */
void AGameState_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGameState_Base, bIsGameOver)
    DOREPLIFETIME(AGameState_Base, PlayerTeams);
    DOREPLIFETIME(AGameState_Base, WaveCount);
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::ClientGameOverUpdated_Implementation()
{
    OnGameOverUpdated.Broadcast();
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::ClientPlayerTeamsUpdated_Implementation()
{
    OnPlayerTeamsUpdated.Broadcast();
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::ClientWaveCountUpdated_Implementation()
{
    OnWaveCountUpdated.Broadcast();
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::OnRep_GameOverUpdated()
{
    ClientGameOverUpdated();
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::OnRep_PlayerTeamsUpdated()
{
    ClientPlayerTeamsUpdated();
}

/**
 * @brief TODO
 * 
 */
void AGameState_Base::OnRep_WaveCountUpdated()
{
    ClientWaveCountUpdated();
}

/**
 * @brief TODO
 * 
 * @return int32 
 */
int32 AGameState_Base::GetWaveCount() const
{
    return WaveCount;
}

/**
 * @brief TODO
 * 
 * @return TArray<FPlayerTeam> 
 */
TArray<FPlayerTeam> AGameState_Base::GetPlayerTeams() const
{
    return PlayerTeams;
}

/**
 * @brief TODO
 * 
 * @param GameOver 
 */
void AGameState_Base::SetGameOver(bool GameOver)
{
    bIsGameOver = GameOver;
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool AGameState_Base::GetGameOver() const
{
    return bIsGameOver;
}
