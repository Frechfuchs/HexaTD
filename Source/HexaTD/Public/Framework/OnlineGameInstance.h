// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FHexSessionResult
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSearchResult OnlineResult;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_NotifyGameInstanceSessionsFound, const TArray<FHexSessionResult>&, SessionSearchResults);

/**
 * TODO: Description
 */
UCLASS()
class HEXATD_API UOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/** TODO */
	UFUNCTION(BlueprintCallable)
	void CreateServer(FName SessionName, int32 NumberOfPlayers, bool IsLan, bool AllowJoinInProgress);

	/** TODO */
	UFUNCTION(BlueprintCallable)
	void FindSessions();

	/** TODO */
	UFUNCTION(BlueprintCallable)
	void JoinHexSession(FHexSessionResult Session);

	/** TODO */
	UFUNCTION(BlueprintCallable)
	FString GetSessionName(FHexSessionResult SessionResult) const;

	/** TODO */
	IOnlineSessionPtr GetSessionInterface() const;

	/** TODO */
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyGameInstanceSessionsFound OnSessionsFound;

protected:
	/** TODO */
	virtual void Init() override;
	/** TODO */
	virtual void OnCreateSessionComplete(FName ServerName, bool Success);
	/** TODO */
	virtual void OnFindSessionComplete(bool Success);
	/** TODO */
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** TODO */
	IOnlineSessionPtr SessionInterface;
	/** TODO */
 	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
