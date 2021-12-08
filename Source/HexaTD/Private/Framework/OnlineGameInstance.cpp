// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/OnlineGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

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
void UOnlineGameInstance::CreateServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Creating Server..."));
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bIsLANMatch = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.NumPublicConnections = 4;

    SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

/**
 * @brief 
 * 
 */
void UOnlineGameInstance::JoinServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Searching Server..."));
    SessionSearch = MakeShareable(new FOnlineSessionSearch());

    SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = 10000;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

/**
 * @brief 
 * 
 * @param ServerName 
 * @param Success 
 */
void UOnlineGameInstance::OnCreateSessionComplete(FName ServerName, bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Create Session success: %d"), Success);
    if (Success)
    {
        GetWorld()->ServerTravel("/Game/HexaTD/Maps/Test?listen");
    }
}

/**
 * @brief 
 * 
 * @param Success 
 */
void UOnlineGameInstance::OnFindSessionComplete(bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Find Session success: %d"), Success);

    if (Success)
    {
        TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
        UE_LOG(LogTemp, Warning, TEXT("Session count: %i"), SearchResults.Num());

        if (SearchResults.Num())
        {
            UE_LOG(LogTemp, Warning, TEXT("Joining Server..."));
            SessionInterface->JoinSession(0, FName("My Session"), SearchResults[0]);
        }
    }
}

/**
 * @brief 
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
