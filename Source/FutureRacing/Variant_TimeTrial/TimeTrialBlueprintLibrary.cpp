// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeTrialBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

TArray<FTimeTrialRecord> UTimeTrialBlueprintLibrary::LoadLeaderboard(const FString& SaveSlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (UTimeTrialSaveGame* SaveGame = Cast<UTimeTrialSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
		{
			return SaveGame->TopTimes;
		}
	}

	// return empty array if no save exists
	return TArray<FTimeTrialRecord>();
}

bool UTimeTrialBlueprintLibrary::DoesLeaderboardExist(const FString& SaveSlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}

FString UTimeTrialBlueprintLibrary::FormatRaceTime(float TimeInSeconds)
{
	const int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
	const float Seconds = FMath::Fmod(TimeInSeconds, 60.0f);

	return FString::Printf(TEXT("%02d:%06.3f"), Minutes, Seconds);
}
