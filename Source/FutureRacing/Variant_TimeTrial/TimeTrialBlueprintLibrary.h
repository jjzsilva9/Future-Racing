// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimeTrialSaveGame.h"
#include "TimeTrialBlueprintLibrary.generated.h"

/**
 * Blueprint function library for time trial utilities
 */
UCLASS()
class FUTURERACING_API UTimeTrialBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Loads the leaderboard from the save game
	 * @param SaveSlotName The name of the save slot to load from
	 * @return Array of leaderboard records, empty if no save exists
	 */
	UFUNCTION(BlueprintCallable, Category="Time Trial|Leaderboard")
	static TArray<FTimeTrialRecord> LoadLeaderboard(const FString& SaveSlotName = TEXT("TimeTrialLeaderboard"));

	/**
	 * Checks if a leaderboard save exists
	 * @param SaveSlotName The name of the save slot to check
	 * @return True if the save exists
	 */
	UFUNCTION(BlueprintPure, Category="Time Trial|Leaderboard")
	static bool DoesLeaderboardExist(const FString& SaveSlotName = TEXT("TimeTrialLeaderboard"));

	/**
	 * Formats a time in seconds to MM:SS.mmm format
	 * @param TimeInSeconds The time to format
	 * @return Formatted time string
	 */
	UFUNCTION(BlueprintPure, Category="Time Trial|Utility")
	static FString FormatRaceTime(float TimeInSeconds);
};
