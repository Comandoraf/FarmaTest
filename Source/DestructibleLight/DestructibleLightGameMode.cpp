// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DestructibleLightGameMode.h"
#include "DestructibleLightHUD.h"
#include "DestructibleLightCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADestructibleLightGameMode::ADestructibleLightGameMode()
	: Super()
{
	// set default pawn class
	DefaultPawnClass = ADestructibleLightCharacter::StaticClass();

	// use our custom HUD class
	HUDClass = ADestructibleLightHUD::StaticClass();
}