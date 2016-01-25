// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "EdaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EDA_API UEdaGameInstance : public UGameInstance
{
	GENERATED_BODY()
		/**
		*	Function to host a game!
		*
		*	@Param		UserID			User that started the request
		*	@Param		SessionName		Name of the Session
		*	@Param		bIsLAN			Is this is LAN Game?
		*	@Param		bIsPresence		"Is the Session to create a presence Session"
		*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
		*/
		bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	
	
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;


	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	UEdaGameInstance();
	/**
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
};
