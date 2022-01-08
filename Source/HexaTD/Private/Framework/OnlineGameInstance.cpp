// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/OnlineGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"

/**
 * @brief 
 * 
 */
void UOnlineGameInstance::Init()
{
    if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
    {
        SessionInterface = SubSystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnJoinSessionComplete);
        }
    }
}

/**
 * @brief 
 * 
 */
void UOnlineGameInstance::CreateServer(FName SessionName, int32 NumberOfPlayers, bool IsLan, bool AllowJoinInProgress)
{
    // TODO: Remove SessionName, its unused
    UE_LOG(LogTemp, Warning, TEXT("Creating Server..."));
    FOnlineSessionSettings SessionSettings;
    SessionSettings.NumPublicConnections = NumberOfPlayers;
    SessionSettings.bIsLANMatch = IsLan;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bAllowJoinInProgress = AllowJoinInProgress;
    SessionSettings.bAllowInvites = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;

    SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

/**
 * @brief TODO
 * 
 */
void UOnlineGameInstance::FindSessions()
{
    UE_LOG(LogTemp, Warning, TEXT("Searching Server..."));
    SessionSearch = MakeShareable(new FOnlineSessionSearch());

    SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = 10000;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

/**
 * @brief TODO
 * 
 * @param Session 
 */
void UOnlineGameInstance::JoinHexSession(FHexSessionResult Session)
{
    UE_LOG(LogTemp, Warning, TEXT("Joining Server..."));
    SessionInterface->JoinSession(0, FName("My Session"), Session.OnlineResult);
}

/**
 * @brief TODO
 * 
 * @param SessionResult 
 * @return FString 
 */
FString UOnlineGameInstance::GetSessionName(FHexSessionResult SessionResult) const
{
    // TODO: Move to UI
    UE_LOG(LogTemp, Warning, TEXT("Ping is: %d"), SessionResult.OnlineResult.PingInMs);
    return SessionResult.OnlineResult.Session.OwningUserName;
}

/**
 * @brief 
 * 
 * @return IOnlineSessionPtr 
 */
IOnlineSessionPtr UOnlineGameInstance::GetSessionInterface() const
{
    if (SessionInterface.IsValid())
    {
        return SessionInterface;
    }
    else
    {
        return nullptr;
    }
}

/**
 * @brief TODO
 * 
 * @param ServerName 
 * @param Success 
 */
void UOnlineGameInstance::OnCreateSessionComplete(FName ServerName, bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Create Session success: %d"), Success);
    if (Success)
    {
        GetWorld()->ServerTravel("/Game/HexaTD/Maps/World_1_Radius?listen");
    }
}

/**
 * @brief TODO
 * 
 * @param Success 
 */
void UOnlineGameInstance::OnFindSessionComplete(bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Find Session success: %d"), Success);

    if (Success)
    {
        TArray<FOnlineSessionSearchResult> OnlineSearchResults = SessionSearch->SearchResults;
        // Create a blueprinttype-copy if SearchResults to pass via broadcast
        TArray<FHexSessionResult> SearchResults;
        UE_LOG(LogTemp, Warning, TEXT("Session count: %i"), OnlineSearchResults.Num());

        for (FOnlineSessionSearchResult Ossr : OnlineSearchResults)
        {
            FHexSessionResult bpsr;
            bpsr.OnlineResult = Ossr;
            SearchResults.Add(bpsr);
        }

        OnSessionsFound.Broadcast(SearchResults);

        if (OnlineSearchResults.Num() > 0)
        {
            // TODO: Move to Join Session
            SessionInterface->JoinSession(0, FName("My Session"), OnlineSearchResults[0]);
        }
    }
}

/**
 * @brief TODO
 * 
 * @param SessionName 
 * @param Result 
 */
void UOnlineGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Joined Session"));

    if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FString ConnectInfo;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
        {
            PController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
        }
    }
}
