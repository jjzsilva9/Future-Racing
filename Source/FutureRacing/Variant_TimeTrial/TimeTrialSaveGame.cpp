// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeTrialSaveGame.h"

bool UTimeTrialSaveGame::AddTime(const FTimeTrialRecord& NewRecord)
{
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
		return false; // time added but didn't make top entries
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
