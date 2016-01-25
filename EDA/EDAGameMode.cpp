// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EDA.h"
#include "EDAGameMode.h"
#include "EDAHUD.h"
#include "EDACharacter.h"

AEDAGameMode::AEDAGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	/* Use our custom Player-Controller Class */
	PlayerControllerClass = AEDAPlayerController::StaticClass();
	

	// use our custom HUD class
	HUDClass = AEDAHUD::StaticClass();
}

UClass* AEDAGameMode::GetDefaultPawnClassForController(AController* InController)
{
	/* Override Functionality to get Pawn from PlayerController */
	AEDAPlayerController* MyController = Cast<AEDAPlayerController>(InController);
	if (MyController)
	{
		return MyController->GetPlayerPawnClass();
	}

	/* If we don't get the right Controller, use the Default Pawn */
	return DefaultPawnClass;
}

