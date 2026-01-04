// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeTrialSaveGame.h"

bool UTimeTrialSaveGame::AddTime(const FTimeTrialRecord& NewRecord)
{
	// // If leaderboard is full, remove the last entry before adding new one
	// if (TopTimes.Num() >= MaxLeaderboardEntries)
	// {
	// 	TopTimes.RemoveAt(TopTimes.Num() - 1);
	// }

	// add the new record to the array
	TopTimes.Add(NewRecord);

	// sort by total time (ascending - lower times first)
	TopTimes.Sort([](const FTimeTrialRecord& A, const FTimeTrialRecord& B)
	{
		return A.TotalTime < B.TotalTime;
	});

	// trim to max entries
	if (TopTimes.Num() > MaxLeaderboardEntries)
	{
		TopTimes.SetNum(MaxLeaderboardEntries);
		// If the new record didn't make the leaderboard, don't mark any as recent
		for (FTimeTrialRecord& Record : TopTimes)
		{
			Record.bIsRecentLap = false;
		}
		return false; // time added but didn't make top entries
	}

	// Mark all as not recent, then mark the new record as recent
	for (FTimeTrialRecord& Record : TopTimes)
	{
		Record.bIsRecentLap = false;
	}
	// Find the record matching the new lap and mark as recent
	for (FTimeTrialRecord& Record : TopTimes)
	{
		if (Record.PlayerName == NewRecord.PlayerName && FMath::IsNearlyEqual(Record.TotalTime, NewRecord.TotalTime))
		{
			Record.bIsRecentLap = true;
			break;
		}
	}

	return true; // time made it into leaderboard
}

bool UTimeTrialSaveGame::IsQualifyingTime(float TotalTime) const
{
	// if leaderboard isn't full yet, any time qualifies
	if (TopTimes.Num() < MaxLeaderboardEntries)
	{
		return true;
	}

	// check if better than the worst time on the leaderboard
	return TotalTime < TopTimes.Last().TotalTime;
}
