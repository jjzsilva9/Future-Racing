#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeTrialGhostSaveGame.h"
#include "TimeTrialGhostCar.generated.h"

UCLASS()
class FUTURERACING_API ATimeTrialGhostCar : public AActor
{
	GENERATED_BODY()

public:
	ATimeTrialGhostCar();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Initialize the ghost with recorded frames
	void InitializeGhost(const TArray<FGhostFrame>& InFrames);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Array of frames to playback
	TArray<FGhostFrame> GhostFrames;

	// Playback timer
	float PlaybackTime = 0.0f;

	// Current frame index
	int32 CurrentFrameIndex = 0;

	// Update ghost transform based on playback time
	void UpdateGhostTransform();
};
