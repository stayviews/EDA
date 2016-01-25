// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "EDAPlayerController.h"
#include "EDACharacter.h"
#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "SoundDefinitions.h"
#include "Online.h"


#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PICKUP		ECC_GameTraceChannel3



#define SHOOTER_SURFACE_Default		SurfaceType_Default
#define SHOOTER_SURFACE_Concrete	SurfaceType1
#define SHOOTER_SURFACE_Dirt		SurfaceType2
#define SHOOTER_SURFACE_Water		SurfaceType3
#define SHOOTER_SURFACE_Metal		SurfaceType4
#define SHOOTER_SURFACE_Wood		SurfaceType5
#define SHOOTER_SURFACE_Grass		SurfaceType6
#define SHOOTER_SURFACE_Glass		SurfaceType7
#define SHOOTER_SURFACE_Flesh		SurfaceType8