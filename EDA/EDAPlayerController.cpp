// Fill out your copyright notice in the Description page of Project Settings.

#include "EDA.h"
#include "EDAPlayerController.h"
#include "Player/EdaPlayerCameraManager.h"



bool AEDAPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

AEDAPlayerController::AEDAPlayerController()
{
	PlayerCameraManagerClass = AEdaPlayerCameraManager::StaticClass();
}

void AEDAPlayerController::BeginPlay() {
	Super::BeginPlay();

}

void AEDAPlayerController::DeterminePawnClass_Implementation()
{
// 	if (IsLocalController()) //Only Do This Locally (NOT Client-Only, since Server wants this too!)
// 	{
// 		/* Load Text File Into String Array */
// 		TArray<FString> TextStrings;
// 		const FString FilePath = FPaths::GameDir() + "Textfiles/PlayerSettings.txt";
// 
// 		/* Use PawnA if the Text File tells us to */
// 		if (TextStrings[0] == "PawnA")
// 		{
// 			ServerSetPawn(PawnToUseA);
// 			return;
// 		}
// 
// 		/* Otherwise, Use PawnB :) */
// 		ServerSetPawn(PawnToUseB);
// 		return;
// 	}
}
// Replication
void AEDAPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEDAPlayerController, MyPawnClass);
}