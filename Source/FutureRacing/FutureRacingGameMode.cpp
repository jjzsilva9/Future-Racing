// Copyright Epic Games, Inc. All Rights Reserved.

#include "FutureRacingGameMode.h"
#include "FutureRacingPlayerController.h"

AFutureRacingGameMode::AFutureRacingGameMode()
{
	PlayerControllerClass = AFutureRacingPlayerController::StaticClass();
}
