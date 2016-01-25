// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "EDAGameMode.generated.h"

UCLASS(minimalapi)
class AEDAGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AEDAGameMode();
	/* Override To Read In Pawn From Custom Controller */
	UClass* GetDefaultPawnClassForController(AController* InController);


};



