// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PlayerState_Base.h"
#include "Framework/GameMode_Base.h"
#include "Net/UnrealNetwork.h"

/**
 * @brief TODO
 * manually called by GameMode_Base. RepNotify doesn't replicate for host.
 */
void APlayerState_Base::OnRep_AllowBuildingUpdated()
{
    ClientAllowBuildingUpdated();
}

/**
 * @brief TODO
 * 
 * @param OutLifetimeProps 
 */
void APlayerState_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerState_Base, bAllowBuilding);
    DOREPLIFETIME(APlayerState_Base, bReadyForWavePhase);
}

/**
 * @brief TODO
 * 
 * @param Ready 
 */
void APlayerState_Base::ClientSetReadyForWavePhase_Implementation(bool Ready)
{
    ServerSetReadyForWavePhase(Ready);
}

/**
 * @brief TODO
 * 
 * @param Ready 
 */
void APlayerState_Base::ServerSetReadyForWavePhase_Implementation(bool Ready)
{
    SetReadyForWavePhase(Ready);
    // Tell GameMode player is ready
    // TODO: Should we keep the GM ref?
    AGameMode_Base* GM = GetWorld()->GetAuthGameMode<AGameMode_Base>();
    GM->HandlePlayerIsReady();
}

/**
 * @brief TODO
 * 
 */
void APlayerState_Base::ClientAllowBuildingUpdated_Implementation()
{
    OnAllowBuildingUpdated.Broadcast();
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool APlayerState_Base::GetAllowBuilding() const
{
	return bAllowBuilding;
}

/**
 * @brief TODO
 * 
 * @param Ready 
 */
void APlayerState_Base::SetAllowBuilding(bool Ready)
{
	bAllowBuilding = Ready;
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool APlayerState_Base::GetReadyForWavePhase() const
{
	return bReadyForWavePhase;
}

/**
 * @brief TODO
 * 
 * @param Ready 
 */
void APlayerState_Base::SetReadyForWavePhase(bool Ready)
{
	bReadyForWavePhase = Ready;
}
