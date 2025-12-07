// Copyright Epic Games, Inc. All Rights Reserved.

#include "FutureRacingWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UFutureRacingWheelRear::UFutureRacingWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}