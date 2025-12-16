// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TimeTrialSaveGame.generated.h"

/**
 * Struct to store a single time trial record
 */
USTRUCT(BlueprintType)
struct FTimeTrialRecord
{
	GENERATED_BODY()

	/** Name of the player who set this record */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	FString PlayerName;

	/** Total race time in seconds */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	float TotalTime;

	/** Best lap time in seconds */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	float BestLapTime;

	/** Date and time when this record was set */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	FDateTime RecordDate;

	/** Default constructor */
	FTimeTrialRecord()
		: PlayerName(TEXT("Player"))
		, TotalTime(0.0f)
		, BestLapTime(0.0f)
		, RecordDate(FDateTime::Now())
	{}
};

/**
 * SaveGame class for storing time trial leaderboard data
 */
UCLASS()
class FUTURERACING_API UTimeTrialSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	/** Array of top times for the leaderboard */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	TArray<FTimeTrialRecord> TopTimes;

	/** Maximum number of entries to keep in the leaderboard */
	UPROPERTY(BlueprintReadWrite, Category = "Time Trial")
	int32 MaxLeaderboardEntries = 10;

	/**
	 * Adds a new time to the leaderboard and sorts it
	 * @param NewRecord The record to add
	 * @return True if the record made it into the top entries
	 */
	bool AddTime(const FTimeTrialRecord& NewRecord);

	/**
	 * Checks if a given time would qualify for the leaderboard
	 * @param TotalTime The time to check
	 * @return True if this time would make it onto the leaderboard
	 */
	bool IsQualifyingTime(float TotalTime) const;
};
