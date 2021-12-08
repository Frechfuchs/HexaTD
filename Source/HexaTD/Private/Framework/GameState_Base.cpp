// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameState_Base.h"

/**
 * @brief 
 * 
 */
/* void AGameState_Base::HandleBeginPlay()
{

} */

/**
 * @brief 
 * 
 */
/* void AGameState_Base::OnRep_MatchState()
{

} */

/**
 * @brief 
 * 
 */
/* void AGameState_Base::BindPlayerReadyDelegates()
{

} */

/**
 * @brief 
 * 
 */
/* void AGameState_Base::OnChangePlayerReady()
{

} */

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool AGameState_Base::IsReadyToStartWavePhase()
{
    return false;
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
