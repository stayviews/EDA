// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "EdaPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class EDA_API AEdaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AEdaPlayerCameraManager();
	virtual void UpdateCamera(float DeltaTime) override;
	
	/* default, hip fire FOV */
	float NormalFOV;

	/* aiming down sight / zoomed FOV */
	float TargetingFOV;

	uint8 bWasCrouched : 1;
	
	
	float CurrentCrouchOffset;

	/* Maximum camera offset applied when crouch is initiated. Always lerps back to zero */
	float MaxCrouchOffsetZ;

	float CrouchLerpVelocity;
	/* Default relative Z offset of the player camera */
	float DefaultCameraOffsetZ;
};
