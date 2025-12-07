// Copyright Epic Games, Inc. All Rights Reserved.

#include "FutureRacingWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UFutureRacingWheelFront::UFutureRacingWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}