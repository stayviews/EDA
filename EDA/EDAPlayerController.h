// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "EDAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EDA_API AEDAPlayerController : public APlayerController
{
	GENERATED_BODY()

		AEDAPlayerController();
		/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
		uint8 bAllowGameActions : 1;
public:	
	   virtual void BeginPlay() override;
	bool IsGameInputAllowed() const;
		FORCEINLINE UClass* GetPlayerPawnClass() { return MyPawnClass; }

		/* Actual Pawn class we want to use */
		UPROPERTY(Replicated)
		TSubclassOf<APawn> MyPawnClass;
		/* First Pawn Type To Use */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My Controller")
		TSubclassOf<APawn> PawnToUseA;

		/* Second Pawn Type To Use */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My Controller")
		TSubclassOf<APawn> PawnToUseB;
		/* Return The Correct Pawn Class Client-Side */
protected:
		
		virtual void DeterminePawnClass_Implementation();
};
