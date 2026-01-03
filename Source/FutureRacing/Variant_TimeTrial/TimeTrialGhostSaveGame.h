

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TimeTrialGhostSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FGhostFrame
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Time = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;
};

UCLASS()
class FUTURERACING_API UTimeTrialGhostSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Ghost")
	TArray<FGhostFrame> GhostFrames;

	UPROPERTY(BlueprintReadWrite, Category = "Ghost")
	float BestTime = 0.0f;
};
