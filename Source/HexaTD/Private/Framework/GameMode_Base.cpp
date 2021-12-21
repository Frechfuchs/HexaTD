// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/GameMode_Base.h"
#include "AI/CheckPoint.h"
#include "AI/Enemy_Base.h"
#include "AI/GoalPoint.h"
#include "AI/SpawnPoint.h"
#include "AI/GraphAStarNavMesh.h"
#include "Framework/Building_Base.h"
#include "Framework/GameState_Base.h"
#include "Framework/OnlineGameInstance.h"
#include "Framework/PlayerController_Base.h"
#include "Framework/PlayerPawn_Base.h"
#include "Framework/PlayerState_Base.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameSession.h"
#include "HexGrid/HexGrid.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

/**
 * @brief TODO
 */
AGameMode_Base::AGameMode_Base() 
{
    // Disable tick
    PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

    // Classes
    DefaultPawnClass = APlayerPawn_Base::StaticClass();
    GameStateClass = AGameState_Base::StaticClass();
    PlayerStateClass = APlayerState_Base::StaticClass();
    PlayerControllerClass = APlayerController_Base::StaticClass();
    SpectatorClass = APlayerPawn_Base::StaticClass();

    // GameMode Config
    //bDelayedStart = true;
}

/**
 * @brief TODO
 * 
 * @param MapName 
 * @param Options 
 * @param ErrorMessage 
 */
void AGameMode_Base::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) 
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

/**
 * @brief TODO
 * 
 * @param NewPlayer 
 */
void AGameMode_Base::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    APlayerController_Base* Player = Cast<APlayerController_Base>(NewPlayer);
    if (Player)
    {
        PlayerControllers.AddUnique(Player);
        Player->ServerInitializePostLogin();
    }
    UE_LOG(LogTemp, Warning, TEXT("Player log in!"));
}

/**
 * @brief TODO
 */
void AGameMode_Base::StartPlay() 
{
	Super::StartPlay();

    // Get references
    UWorld* World = GetWorld();
    GameInstance = GetGameInstance<UOnlineGameInstance>();
    GameState = GetGameState<AGameState_Base>();
    HexGrid = Cast<AHexGrid>(UGameplayStatics::GetActorOfClass(World, AHexGrid::StaticClass()));
    GoalPoint = UGameplayStatics::GetActorOfClass(World, AGoalPoint::StaticClass());
    SpawnPoint = Cast<ASpawnPoint>(UGameplayStatics::GetActorOfClass(World, ASpawnPoint::StaticClass()));
    UGameplayStatics::GetAllActorsOfClass(World, ACheckPoint::StaticClass(), CheckPoints);
    AGraphAStarNavMesh* GraphAStarNavMesh = Cast<AGraphAStarNavMesh>(UGameplayStatics::GetActorOfClass(World, AGraphAStarNavMesh::StaticClass()));

    // ErrorHandling
    if (!GameInstance) UE_LOG(LogTemp, Error, TEXT("GAMEINSTANCE NOT FOUND"));
    if (!GameState) UE_LOG(LogTemp, Error, TEXT("GAMESTATE NOT FOUND"));
    if (!HexGrid) UE_LOG(LogTemp, Error, TEXT("HEXGRID NOT FOUND"));
    if (!GoalPoint) UE_LOG(LogTemp, Error, TEXT("GOALPOINT NOT FOUND"));
    if (!SpawnPoint) UE_LOG(LogTemp, Error, TEXT("SPAWNPOINT NOT FOUND"));
    if (!GraphAStarNavMesh) UE_LOG(LogTemp, Error, TEXT("NAVMESH NOT FOUND"))

    // Block System Grid Space
    OccupieSystemGridSpaces();

    // Set NavMesh Target
    if (GraphAStarNavMesh && HexGrid) 
    {
        GraphAStarNavMesh->SetHexGrid(HexGrid);
    }

    // Setup GameState
    if (GameState) GameState->CreateTeams();
}

/**
 * @brief TODO
 */
void AGameMode_Base::OnMatchStateSet() 
{
    Super::OnMatchStateSet();
	UE_LOG(LogTemp, Warning, TEXT("MatchState is: %s"), *MatchState.ToString());
    
    if (MatchState == MatchStateWaitToStart)
    {
        return;
    }

    if (MatchState == MatchStateInProgress)
    {
        // Match starts and switches into Build Phase
        FTimerHandle TimerHandle;
        // Wait for the last player to init, then start
        float Delay = 5.f;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AGameMode_Base::HandleMatchStateInProgress, Delay, false);
        return; 
    }

    if (MatchState == MatchStateBuildingPhase)
    {
        HandleMatchStateBuildingPhase();
        return;
    }

    if (MatchState == MatchStateWavePhase)
    {
        HandleMatchStateWavePhase();
        return;
    }

    if (MatchState == MatchStateGameOver)
    {
        HandleMatchStateGameOver();
    }
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool AGameMode_Base::ReadyToStartMatch_Implementation() 
{
    auto ActivePlayers = GetNumPlayers();
    auto MaxPlayers = 8; // Fallback value
    // TODO: This needs refactoring: The GameInstance is not available when the Game initializes.
    // So we wait until it is available.
    if (GameInstance)
    {
        // TODO: When it is available and we are testing with only one player, MaxPlayer should equal to 1:
        MaxPlayers = 1;
        auto SessionInterface = GameInstance->GetSessionInterface();
        if (SessionInterface)
        {
            // TODO: Refactor the magic FName("My Session") to something configurable.
            auto SessionSettings = SessionInterface->GetSessionSettings(FName("My Session"));
            if (SessionSettings)
            {
                // TODO: When the Setting is found, we can safely set MaxPlayer by the number to host set:
                MaxPlayers = SessionSettings->NumPublicConnections;
                UE_LOG(LogTemp, Warning, TEXT("NumPlayers: %d, MaxPlayers: %d"), NumPlayers, MaxPlayers);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("NO SESSION SETTINGS"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NO SESSION INTERFACE"));
        }
    }
    //GetNumPlayers() == GameInstance->GetSessionInterface()->GetSessionSettings(FName("My Session")).NumPublicConnections;
    return ActivePlayers >= MaxPlayers;
	//return GetNumPlayers() == GameSession->MaxPlayers;
}

/**
 * @brief TODO
 */
void AGameMode_Base::HandleMatchStateInProgress() 
{
	UE_LOG(LogTemp, Warning, TEXT("Starting BuildingPhase"));
    SetMatchState(MatchStateBuildingPhase);
}

/**
 * @brief TODO
 */
void AGameMode_Base::HandleMatchStateBuildingPhase() 
{
    for (APlayerController_Base* PC : PlayerControllers)
    {
        APlayerState_Base* PlayerState = PC->GetPlayerState<APlayerState_Base>();
        if (PlayerState)
        {
            // Update PlayerStates AllowBuilding
            PlayerState->SetAllowBuilding(true);
            PlayerState->ResourceAmount += 5;
            PlayerState->SetReadyForWavePhase(false);
            // Call RepNotify for server manually, it won't be called on the server by the engine
            // TODO: Maybe only call for host
            PlayerState->OnRep_AllowBuildingUpdated();
            PlayerState->OnRep_ResourceAmountUpdated();
        }
    }
    if (GameState)
    {
        GameState->WaveCount++;
        GameState->OnRep_WaveCountUpdated();
    }
}

// TODO: Refactor! Mostly identical with HandleMatchStateBuildingPhase
/**
 * @brief TODO
 */
void AGameMode_Base::HandleMatchStateWavePhase() 
{
    // Update players
	ForbidPlayersBuilding();

    // Start wave
    if (SpawnPoint)
    {
        bWaveFinishedSpawn = false;
        SpawnPoint->StartWave();
    }
}

/**
 * @brief TODO
 */
void AGameMode_Base::HandleMatchStateGameOver()
{
    ForbidPlayersBuilding();
    GameState->SetGameOver(true);
    GameState->OnRep_GameOverUpdated();
}

/**
 * @brief TODO
 * 
 */
void AGameMode_Base::HandlePlayerIsReady()
{
    if (MatchState == MatchStateBuildingPhase)
    {
        // TODO Refactor
        bool AtLeastOnePlayerNotReady = false;
        for (APlayerController_Base* PC : PlayerControllers)
        {
            if (!PC->GetPlayerState<APlayerState_Base>()->GetReadyForWavePhase())
            {
                AtLeastOnePlayerNotReady = true;
                break;
            }
        }

        // If all players are ready
        if (!AtLeastOnePlayerNotReady)
        {
            SetMatchState(MatchStateWavePhase);
        }
    }
}

/**
 * @brief TODO
 */
void AGameMode_Base::HandleWaveFinishedSpawn() 
{
	bWaveFinishedSpawn = true;
}

/**
 * @brief TODO
 * 
 * @param TeamID 
 * @param LivesCount 
 */
void AGameMode_Base::TeamLosingLives(int TeamID, int LivesCount)
{
    bool IsGameOver = false;
    CheckForWaveFinished();
    GameState->TeamLosingLives(TeamID, LivesCount, IsGameOver);
    GameState->OnRep_PlayerTeamsUpdated();
    if (IsGameOver)
    {
        SetMatchState(MatchStateGameOver);
    }
}

/**
 * @brief TODO
 */
void AGameMode_Base::OccupieSystemGridSpaces() 
{
    // TODO: Check for success!
	if (HexGrid)
    {
        bool bSuccess = false;
        // Block SpawnPoint
        if (SpawnPoint) HexGrid->OccupieGridSpacePlayerOnly(SpawnPoint->GetActorLocation(), bSuccess);
        // Block GoalPoint
        if (GoalPoint) HexGrid->OccupieGridSpacePlayerOnly(GoalPoint->GetActorLocation(), bSuccess);
        // Block CheckPoints
        for (AActor* CheckPoint : CheckPoints)
        {
            HexGrid->OccupieGridSpacePlayerOnly(CheckPoint->GetActorLocation(), bSuccess);
        }
    }
}

/**
 * @brief TODO
 */
void AGameMode_Base::CheckForWaveFinished() 
{
    // Check if any Enemy is still alive
	auto Enemy = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy_Base::StaticClass());
    if (!Enemy && bWaveFinishedSpawn)
    {
        SetMatchState(MatchStateBuildingPhase);
    }
}

/**
 * @brief TODO
 */
void AGameMode_Base::ForbidPlayersBuilding()
{
    for (APlayerController_Base* PC : PlayerControllers)
    {
        APlayerState_Base* PlayerState = PC->GetPlayerState<APlayerState_Base>();
        if (PlayerState)
        {
            // Update PlayerStates AllowBuilding
            PlayerState->SetAllowBuilding(false);
            // Call RepNotify for server manually, it won't be called on the server by the engine
            PlayerState->OnRep_AllowBuildingUpdated();
        }
    }
}

/**
 * @brief TODO
 * 
 * @return true 
 * @return false 
 */
bool AGameMode_Base::IsBuildingPhase()
{
    return MatchState == MatchStateBuildingPhase;
}

/**
 * @brief TODO
 */
void AGameMode_Base::ResetGame()
{
    if (MatchState == MatchStateGameOver)
    {
        // Reset everything
        UWorld* World = GetWorld();
        // Destroy Actors
        TArray<AActor*> Enemies;
        TArray<AActor*> Buildings;
        UGameplayStatics::GetAllActorsOfClass(World, AEnemy_Base::StaticClass(), Enemies);
        UGameplayStatics::GetAllActorsOfClass(World, ABuilding_Base::StaticClass(), Buildings);

        for (AActor* Enemy : Enemies)
        {
            Enemy->Destroy();
        }
        for (AActor* Building : Buildings)
        {
            bool Success;
            HexGrid->FreeOccupiedGridSpace(Building->GetActorLocation(), Success);
            if (Success)
            {
                Building->Destroy();
            }
        }

        // Reset GameState
        // TODO: Multiple teams
        GameState->SetTeamLives(0, 20);
        GameState->SetGameOver(false);
        GameState->WaveCount = 0;
        
        // GameState Replication for host
        GameState->OnRep_GameOverUpdated();
        GameState->OnRep_PlayerTeamsUpdated();

        // Reset PlayerStates
        for (APlayerController_Base* PC : PlayerControllers)
        {
            APlayerState_Base* PlayerState = PC->GetPlayerState<APlayerState_Base>();
            if (PlayerState)
            {
                PlayerState->ResourceAmount = 0;
            }
        }

        // Start in BuildingPhase
        SetMatchState(MatchStateBuildingPhase);
    }
}
